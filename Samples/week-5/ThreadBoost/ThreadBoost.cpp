#include <Windows.h>
#include <stdio.h>
#include "..\KMDF ThreadBooster\ThreadBoosterCommon.h"

int main(int argc, const char* argv[])
{
	if (argc < 3) {
		printf("Uso: threadboost <tid> <prioridad>\n");
		return 0;
	}

	int tid = atoi(argv[1]);
	int priority = atoi(argv[2]);

	HANDLE hDevice = CreateFile(L"\\\\.\\ThreadBooster", GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error abriendo el device (%u)\n", GetLastError());
		return 1;
	}

	ThreadData data;
	data.Priority = priority;
	data.ThreadId = tid;

	DWORD bytes;
	BOOL ok = DeviceIoControl(hDevice, IOCTL_SET_PRIORITY, &data, sizeof(data), nullptr, 0, &bytes, nullptr);
	if (!ok) {
		printf("Error en DeviceIoControl (%u)\n", GetLastError());
		return 1;
	}

	printf("Success!!!\n");

	CloseHandle(hDevice);
}

