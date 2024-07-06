//Necessary Headers
#include <ntddk.h> // Contains definitions for all core Windows Kernel drivers, such as DRIVER_INITIALIZE
#include <wdf.h> // Contains definitions for Windows Driver Framework drivers

// In this example, 2 callbacks will be used: the first is DriverEntry, the second is KmdfHelloWorldEvtDeviceAdd
// - DriverEntry: Entry point
// - KmdfHelloWorldEvtDeviceAdd: Triggered when a device is detected.

// Callback declarations
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd; // This is the entry point for the driver when a device is added. Here the PnP (Plug and Play) manager reports the existence of a device

// We will start by writing the DriverEntry code
NTSTATUS   // Data type used to return the status of an operation (successful, error, ...)
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) { //_In_ This indicates that the parameter is input, meaning the function should not modify the value, only read it
    // status will be the variable to indicate whether the operation was successful or failed
    NTSTATUS status = STATUS_SUCCESS;

    // Driver Configuration Object, which will be used later when we utilize EvtDeviceAdd
    WDF_DRIVER_CONFIG config;

    // Directly from Driver Entry, when everything is initialized, it should print KmdfHelloWorld: DriverEntry
    // It would print a message to the debug console.
    // DPFLTR_IHVDRIVER_ID: This is a filter identifier indicating the source of the debug message
    // DPFLTR_INFO_LEVEL: Indicates that the message is informational level.
    // Message: Text to display
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: DriverEntry\n"));

    // Initialize the driver configuration object to register the entry point for the callback or the EvtDeviceAdd callback: KmdfHelloWorldEvtDeviceAdd
    // parameter1: Pointer to the driver configuration object.
    // parameter2: Entry point for the EvtDeviceAdd callback
    WDF_DRIVER_CONFIG_INIT(&config, KmdfHelloWorldEvtDeviceAdd); // WDF_DRIVER_CONFIG_INIT Macro

    // Create the Driver Object
    status = WdfDriverCreate(DriverObject, // Pointer to the driver object
                             RegistryPath, // This pointer is the location in the Windows registry where the configuration data specific to this driver is stored
                             WDF_NO_OBJECT_ATTRIBUTES, // Indicates that no additional attributes are being used
                             &config, // Pointer to the configuration object initialized in line 32
                             WDF_NO_HANDLE // Optional, indicating that no additional environment handle is required
    );
    return status;
}

// The callback should return a STATUS (STATUS_SUCCESS, error status value...)
NTSTATUS
KmdfHelloWorldEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit) { // _Inout_ indicates that the parameter is input and output, so the function can read and modify it.
    // We are not using the DriverObject called Driver, so it must be marked as UNREFERENCED_PARAMETER to avoid an error
    UNREFERENCED_PARAMETER(Driver);

    // status will be the variable to indicate whether the operation was successful or failed
    NTSTATUS status;

    // This variable will represent the device object
    WDFDEVICE hDevice;

    // Now we print HelloWorld again but when a device is detected, so we will show this other message: "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));

    // Create the Device Object
    status = WdfDeviceCreate(&DeviceInit,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &hDevice // Pointer to the variable representing the device object
                            );
    return status;
}
