# KMDF Thread Booster Driver Sample

The current driver sample sets the thread priority between values 1 to 31 regardless of its base priority.

## KMDF ThreadBooster

The project called *KMDF ThreadBooster* contains the source code related to the driver itself.

## ThreadBoost

The project named *ThreadBoost* contains the source code for the client-side. It runs from the command line.

## Build, Deployd and Install

### Using Visual Studio

For building, deploying, and installing the driver, we will reference the article created by Microsoft on how to create a kernel-mode driver. This article guides you through writing a small Universal Windows driver using the Kernel-Mode Driver Framework (KMDF) and then deploying and installing it on a separate computer. 

The part we are interested in is found in the sections *Build the driver*, *Deploy the driver*, and *Install the driver*.

Write a Hello World Windows Driver (KMDF) [HelloWorld](https://learn.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/writing-a-very-small-kmdf--driver).

### Using CreateServiceAPI

We will use a built-in Windows tool for managing services called *Sc.exe*. We will use it to install and then load the driver.

#### Install the driver

Open an elevated command window and type the following:

```
sc create <drivername> type= kernel binPath= <PATH-sys>
```
* *drivername*: Specify a name for the driver.
* *PATH-sys*: Should be the path on your system where the SYS file resides.

For example:
```
sc create ThreadBooster type= kernel binPath= c:\dev\KMDFThreadBooster.sys
```

#### Load the driver

On 64 bit systems drivers must be signed, so to test the driver, you need to enable test signing. With an elevated command window:

```
bcdedit /set testsigning on
```

This command requires a reboot to take effect.

## Run the driver

To run or load the driver, open a command prompt with administrator privileges and execute:

```
sc start <drivername>
```

* *drivername*: Name of the driver installed in the system.

When the driver is running, open a command prompt in the folder where the *ThreadBoost.exe* executable is located. Then, execute:
```
ThreadBoost <tid> <priority>.
```
* *tid*: Thread ID.
* *priority*: Thread priority.

## Unload the driver

The following command is to unload the driver. Open an elevated command window and type the following:
```
sc stop <drivername>
```
* *drivername*: Name of the driver installed in the system.

