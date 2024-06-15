#pragma once

struct ProcessEnhacedInput {
	ULONG ProcessId;
};

struct ProcessEnhacedOutput {
	HANDLE hProcess;
};

#define IOCTL_OPEN_PROCESS CTL_CODE(0x8000, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)