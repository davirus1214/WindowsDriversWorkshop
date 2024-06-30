#pragma once

struct ProcessPowerInput {
	ULONG ProcessId;
};

struct ProcessPowerOutput {
	HANDLE hProcess;
};

#define IOCTL_OPEN_PROCESS CTL_CODE(0x8000, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)