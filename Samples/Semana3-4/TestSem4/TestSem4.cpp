// TestSem4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\Semana3KmdfProcess\ProcessEnhacedCommon.h"


void ProcessModules(HANDLE hProcess) {
	HMODULE h[4096];
	DWORD needed;
	if (!EnumProcessModulesEx(hProcess, h, sizeof(h), &needed, LIST_MODULES_ALL)) {
		return;
	}

	DWORD count = needed / sizeof(HMODULE);
	printf("%u modulos\n", count);

	WCHAR name[MAX_PATH];
	for (int i = 0; i < count; i++) {
		printf("HModule: 0x%p", h[i]);
		if (GetModuleBaseName(hProcess, h[i], name, _countof(name))) {
			printf(" %ws", name);
		}
		printf("\n");
	}
}


int main(int argc, const char* argv[])
{
	if (argc < 2) {
		printf("Uso: testsem4 <pid>\n");
		return 0;
	}

	int pid = atoi(argv[1]);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess) {
		ProcessModules(hProcess);
		CloseHandle(hProcess);
		return 0;
	}

	printf("Fallo al abrir el proceso con OpenProcess (%u)\n", GetLastError());

	HANDLE hDevice = CreateFile(L"\\\\.\\ProcessEnhaced", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error abriendo el Device (%u)\n", GetLastError());
		return 1;
	}

	ProcessEnhacedInput input;
	input.ProcessId = pid;

	ProcessEnhacedOutput output;
	DWORD bytes;
	BOOL ok = DeviceIoControl(hDevice, IOCTL_OPEN_PROCESS, &input, sizeof(input), &output, sizeof(output), &bytes, nullptr);
	if (!ok) {
		printf("Error: %u\n", GetLastError());
		return 1;
	}

	printf("Success! \n");

	ProcessModules(output.hProcess);
	CloseHandle(output.hProcess);

	CloseHandle(hDevice);
}

