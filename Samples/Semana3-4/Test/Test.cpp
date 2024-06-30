// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\ProcessPower\ProcessPowerCommon.h"

void DumpProcessModules(HANDLE hProcess) {
	HMODULE h[4096];
	DWORD needed;
	if (!EnumProcessModulesEx(hProcess, h, sizeof(h), &needed, LIST_MODULES_ALL))
		return;

	DWORD count = needed / sizeof(HMODULE);
	printf("%u modules\n", count);

	WCHAR name[MAX_PATH];
	for (int i = 0; i < count; i++) {
		printf("HModule: 0x%p", h[i]);
		if (GetModuleBaseName(hProcess, h[i], name, _countof(name))) {
			printf(" %ws", name);
		}
		printf("\n");
	}

}

int main(int argc, const char* argv[]){
	if (argc < 2) {
		printf("Usage: test <pid>\n");
		return 0;
	}

	int pid = atoi(argv[1]);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess) {
		DumpProcessModules(hProcess);
		CloseHandle(hProcess);
		return 0;
	}

	printf("Failed to open process with OpenProcess (%u)\n", GetLastError());

	HANDLE hDevice = CreateFile(L"\\\\.\\ProcessPower", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error opening device (%u)\n", GetLastError());
		return 1;
	}

	ProcessPowerInput input;
	input.ProcessId = pid;

	ProcessPowerOutput output;
	DWORD bytes;
	BOOL ok = DeviceIoControl(hDevice, IOCTL_OPEN_PROCESS, &input, sizeof(input), &output, sizeof(output), &bytes, nullptr);
	if (!ok) {
		printf("Error %u\n", GetLastError());
		return 1;
	}

	printf("Success!\n");

	DumpProcessModules(output.hProcess);

	CloseHandle(output.hProcess);

	CloseHandle(hDevice);
}
