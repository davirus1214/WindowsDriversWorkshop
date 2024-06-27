# KMDF Thread Booster Driver Sample

The current driver sample sets the thread priority between values 1 to 31 regardless of its base priority.

## KMDF ThreadBooster

The project called *KMDF ThreadBooster* contains the source code related to the driver itself.

## ThreadBoost

The project named *ThreadBoost* contains the source code for the client-side. It runs from the command line.

## Build, Deployd and Install

For building, deploying, and installing the driver, we will reference the article created by Microsoft on how to create a kernel-mode driver. This article guides you through writing a small Universal Windows driver using the Kernel-Mode Driver Framework (KMDF) and then deploying and installing it on a separate computer. 

The part we are interested in is found in the sections *Build the driver*, *Deploy the driver*, and *Install the driver*.

Write a Hello World Windows Driver (KMDF) [HelloWorld](https://learn.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/writing-a-very-small-kmdf--driver).


## Run the sample

To run the driver, open a command prompt with administrator privileges and execute:

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
