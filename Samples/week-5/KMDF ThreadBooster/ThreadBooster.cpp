#include <ntifs.h>
#include <ntddk.h>
#include "ThreadBoosterCommon.h"

//Prototipos
void ThreadBoosterUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS ThreadBoosterCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ThreadBoosterDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = ThreadBoosterUnload;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = ThreadBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = ThreadBoosterCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ThreadBoosterDeviceControl;

	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\ThreadBooster");
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Fallo en IoCreateDevice (0x%X)\n", status));
		return status;
	}
	
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ThreadBooster");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Fallo en IoCreateSymbolicLink (0x%X)\n", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	return STATUS_SUCCESS;

}

void ThreadBoosterUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\ThreadBooster");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS ThreadBoosterCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS ThreadBoosterDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;


	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case IOCTL_SET_PRIORITY:
			if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ThreadData)) {
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
			ThreadData* data = (ThreadData*)Irp->AssociatedIrp.SystemBuffer;
			if (data == nullptr) {
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			if (data->Priority < 1 || data->Priority > 31) {
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			

			PETHREAD Thread;
			status = PsLookupThreadByThreadId(UlongToHandle(data->ThreadId), &Thread);
			if (!NT_SUCCESS(status)) {
				break;
			}

			KeSetPriorityThread((PKTHREAD)Thread, data->Priority);
			ObDereferenceObject(Thread);

			break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;

}
