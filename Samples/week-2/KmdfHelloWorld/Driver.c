//HEaders necesarios
#include <ntddk.h> // Contiene definiciones de todos los drivers del core Windows Kernel. como por ej DRIVER_INITIALIZE
#include <wdf.h> // Contiene definiciones de drivers del Windows Driver Framework

// Es este ejemplo se utilizarán 2 callbacks: el primero es DriverEntry, el segundo es KmdfHelloWorldEvtDeviceAdd
// - DriverEntry: Punto de entrada
// - KmdfHelloWorldEvtDeviceAdd: Se activaria cuando detecta un dispositivo.

// Declaraciones de los callbacks
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd; //es el punto de entrada para el driver cuando se agrega un dispositivo. Aqui el PnP (Plug and Play) manager reposta sobre la existencia de un dispositivo

// Vamos a empezar escribiendo el código del DriverEntry
NTSTATUS   // tipo de dato, que lo utilizamos para devolver el estado de una operación (exitosa, error, ...)
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) { //_In_ Esto es para indicar que el parámetro es de entrada, o sea, la funcion no debería modificar el valor, solamente lo lee
	// status sera la variable para indicar el estado si fue exitoso o fallido
	NTSTATUS status = STATUS_SUCCESS;

	// Driver Configuration Object, el cual nos sirve para cuando utilicemos más adelante EvtDeviceAdd 
	WDF_DRIVER_CONFIG config;

	// Directamente desde Driver Entry, cuando todo se inicializa, debe imprimir KmdfHelloWorld: DriverEntry
	// Imprimiria un mensaje en la consola de depuracion. 
	// DPFLTR_IHVDRIVER_ID: Este es un identificador de filtro que indica el origen del mensaje de depuración
	// DPFLTR_INFO_LEVEL: indica que el mensaje es de nivel informativo.
	// Mensaje: texto a mostrar
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: DriverEntry\n"));

	// Inicializamos el driver configuration object para registrar el punto de entrada para la devolución de la llamada o el EvtDeviceAdd callback: KmdfHelloWorldEvtDeviceAdd
	// parametro1: puntero al objeto de configuracion del Driver. 
	// parametro2: es el punto de entrada para la devolución de llamada EvtDeviceAdd 
	WDF_DRIVER_CONFIG_INIT(&config, KmdfHelloWorldEvtDeviceAdd); // Macro WDF_DRIVER_CONFIG_INIT


	// Crea el Driver Object
	status = WdfDriverCreate(DriverObject, // puntero al driver object
							 RegistryPath, // este puntero es la ubicacion en el registro de windows donde se almacenan los datos de configuracion específicos de este driver
							 WDF_NO_OBJECT_ATTRIBUTES, // es para indicar que no se estan utilizando atributos adicionales
							 &config, // puntero al objeto de configuracion que se inicializó en la linea 32
							 WDF_NO_HANDLE // Opcional, y le indicamos que no requiere ningún id de entorno adicional.
	);
	return status;


}


// el callback debe retornar un STATUS ( STATUS_SUCCESS, Error status value....)
NTSTATUS
KmdfHelloWorldEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit) { // _Inout_ es para indicar que el parametro es de entrada y salida, entonces la funcion puede leer y modificar.
	// No estariamos utilizando el DriverObject llamado Driver entonces hay que colocarlo como UNREFERENCED_PARAMETER para que no de error
	UNREFERENCED_PARAMETER(Driver);

	// status sera la variable para indicar el estado si fue exitoso o fallido
	NTSTATUS status;

	// esta variable representará el device object
	WDFDEVICE hDevice;

	//Ahora volvemos a imprimir HelloWord pero cuando se detecta un dispositivo, por eso moestraremos este otro mensaje: "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));

	// Crea el Device Object
	status = WdfDeviceCreate(&DeviceInit,
							 WDF_NO_OBJECT_ATTRIBUTES,
							 &hDevice // puntero al la variable que representa el deviceobject
							);
	return status;

}