#pragma once

#define IOCTL_SET_PRIORITY CTL_CODE(0x8000,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)

struct ThreadData {
	ULONG ThreadId;
	int Priority;
};
