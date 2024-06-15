#include <ntddk.h>
#include "ProcessEnhacedCommon.h"

//Prototipo
void Semana3KmdfProcessUnload(PDRIVER_OBJECT);
NTSTATUS ProcessEnhacedCreateClose(PDEVICE_OBJECT, PIRP);
NTSTATUS ProcessEnhacedDeviceControl(PDEVICE_OBJECT, PIRP);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {

	KdPrint(("Semana3KmdfProcess: DriverEntry\n"));
	KdPrint(("Direccion del Registro: %wZ\n", RegistryPath));

	DriverObject->DriverUnload = Semana3KmdfProcessUnload;

	RTL_OSVERSIONINFOW vi = { sizeof(vi) };
	NTSTATUS status = RtlGetVersion(&vi);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Semana3KmdfProcess Failed in RtlGetVersion: (0x%X)\n", status));
		return status;
	}

	KdPrint(("Windows version: %u.%u.%u\n", vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber));

	DriverObject->MajorFunction[IRP_MJ_CREATE] = ProcessEnhacedCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = ProcessEnhacedCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcessEnhacedDeviceControl;
	

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\ProcessEnhaced");

	PDEVICE_OBJECT DeviceObject;
	status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Fallo en IoCreateDevice (0x%X)\n", status));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ProcessEnhaced");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(DeviceObject);
		KdPrint(("Fallo en IoCreateSymbolicLink (0x%X)\n", status));
		return status;
	}


	return STATUS_SUCCESS;
}

void Semana3KmdfProcessUnload(PDRIVER_OBJECT DriverObject) {
	KdPrint(("Semana3KmdfProcess: DriverUnload\n"));
	
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ProcessEnhaced");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS ProcessEnhacedCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp ) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}

NTSTATUS ProcessEnhacedDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	ULONG len = 0;
	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case IOCTL_OPEN_PROCESS:
			if(stack->Parameters.DeviceIoControl.Type3InputBuffer == nullptr || Irp->UserBuffer == nullptr ) {
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ProcessEnhacedInput) || stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(ProcessEnhacedOutput)) {
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			ProcessEnhacedInput* input = (ProcessEnhacedInput*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
			ProcessEnhacedOutput* output = (ProcessEnhacedOutput*)Irp->UserBuffer;

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

