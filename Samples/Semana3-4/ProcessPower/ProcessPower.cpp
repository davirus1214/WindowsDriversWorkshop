#include <ntddk.h>
#include "ProcessPowerCommon.h"

//Prototipo
void ProcessPowerUnload(PDRIVER_OBJECT);
NTSTATUS ProcessPowerCreateClose(PDEVICE_OBJECT, PIRP);
NTSTATUS ProcessPowerDeviceControl(PDEVICE_OBJECT, PIRP);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	KdPrint(("ProcessPower: DriverEntry\n"));
	KdPrint(("Registry path: %wZ\n", RegistryPath));

	DriverObject->DriverUnload = ProcessPowerUnload;

	RTL_OSVERSIONINFOW vi = { sizeof(vi) };
	NTSTATUS status = RtlGetVersion(&vi);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed in RtlGetVersion (0x%X)\n", status));
		return status;
	}

	KdPrint(("Windows version: %u.%u.%u\n", vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber));

	DriverObject->MajorFunction[IRP_MJ_CREATE] = ProcessPowerCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = ProcessPowerCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcessPowerDeviceControl;

	//Opcion 1
	//UNICODE_STRING devName;
	//RtlInitUnicodeString(&devName, L"\\Device\\ProcessPower");

	//Opcion 2
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\ProcessPower");

	PDEVICE_OBJECT DeviceObject;
	status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed in IoCreateDevice (0x%X)\n", status));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ProcessPower");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		KdPrint(("Failed in IoCreateSymbolicLink (0x%X)\n", status));
		return status;
	}

	return STATUS_SUCCESS;
}

void ProcessPowerUnload(PDRIVER_OBJECT DriverObject) {
	KdPrint(("ProcessPower: Unload\n"));
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ProcessPower");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS ProcessPowerCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}

NTSTATUS ProcessPowerDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	ULONG len = 0;
	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case IOCTL_OPEN_PROCESS:
			if (stack->Parameters.DeviceIoControl.Type3InputBuffer == nullptr || Irp->UserBuffer == nullptr) {
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ProcessPowerInput) || stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(ProcessPowerOutput)) {
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			ProcessPowerInput* input = (ProcessPowerInput*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
			ProcessPowerOutput* output = (ProcessPowerOutput*)Irp->UserBuffer;

			OBJECT_ATTRIBUTES attr;
			InitializeObjectAttributes(&attr, nullptr, 0, nullptr, nullptr);
			CLIENT_ID cid = { 0 };
			cid.UniqueProcess = UlongToHandle(input->ProcessId);
			status = ZwOpenProcess(&output->hProcess, PROCESS_ALL_ACCESS, &attr, &cid);
			if (NT_SUCCESS(status)) {
				len = sizeof(output);
			}

			break;
	}
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = len;
	IoCompleteRequest(Irp, 0);
	return status;
}

