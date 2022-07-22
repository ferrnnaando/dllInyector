#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

DWORD GetProcId(const char*);

int main() {
	std::string procNameString, dllPathString;
	std::cout << "enter a dll path: ";
	std::cin >> dllPathString;
	std::cout << "enter a exe name: ";
	std::cin >> procNameString;

	const char* procName = procNameString.c_str();
	const char* dllPath = dllPathString.c_str();

	DWORD procId = 0;

	if (!procId) {
		procId = GetProcId(procName);
		Sleep(50);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (hProc && hProc != INVALID_HANDLE_VALUE) {
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);

		HANDLE RemoteThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
	
		if (RemoteThread) {
			CloseHandle(RemoteThread);
		}
	}

	if (hProc) {
		CloseHandle(hProc);
	}

	return 0;
}

DWORD GetProcId(const char* procName) {
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
					do {
				if (!_stricmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					}
				} while (Process32Next(hSnap, &procEntry));
			}

		if (hSnap) {
			CloseHandle(hSnap);
		}

		return procId;
	}
}