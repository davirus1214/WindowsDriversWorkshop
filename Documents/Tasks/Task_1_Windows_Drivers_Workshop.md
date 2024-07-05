# Windows Drivers Workshop

**Due Date:** June 20, 2024

**Professors:** 
- [Leonardo David Fariña Ozamis](https://github.com/LeonardoFarina)
- [David Esteban González Agüero](https://github.com/davirus1214)


## Build, Deploy, Install and WinDBG

### Description:
The objective of this assignment is to make use of the Windows Drivers Kit environment. To do this, you must follow its correct installation and configuration. This driver must be built, deployed, and installed on a virtual machine with Windows 10/11. Additionally, Windbg must be used to connect the driver and view the student's name in the memory section.

# Table of Contents
1. [Development Environment Installation](#development-environment-installation)
2. [Project Configuration](#project-configuration)
3. [Add the Driver Code](#add-the-driver-code)
4. [Modify the Driver](#modify-the-driver)
5. [Build and Verify](#build-and-verify)
6. [Target Virtual Machine Configuration](#target-virtual-machine-configuration)
7. [Connect to the Target Virtual Machine](#connect-to-the-target-virtual-machine)
8. [Configure Deployment](#configure-deployment)
9. [Driver Installation](#driver-installation)
10. [Disable the Driver](#disable-the-driver)
11. [Connect WinDBG](#connect-windbg)
12. [Enable the Driver](#enable-the-driver)
13. [View Name](#view-name)
14. [Resources](#resources)

### **Development Environment Installation**

- Follow the "Installation Guide" found on Documents.

### **Project Configuration**

- Create a new project from the Visual Studio templates called Kernel Mode Driver, Empty (KMDF).
- Name it KmdfHelloWorld.
- Check "Place solution and project in the same directory".
- Create Project.

### **Add the Driver Code**

- In the "Solution explorer", right-click on our KmdfHelloWorld project, the one that is expandable. Not on "Solution KmdfHelloWorld".
- Click on "Add", then "New Item", and name it "Driver.c".
- Now copy the code from [HelloWorld/Driver.c](https://github.com/davirus1214/WindowsDriversWorkshop/blob/main/Samples/Semana2/KmdfHelloWorld/Driver.c) and paste it into our "Driver.c" file.

### **Modify the Driver**

- In our "Driver.c" file at lines 27 and 61, you will find:

    ```C:
        27 - KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: DriverEntry\n"));
        61 - KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));
    ```

- You should modify the strings to print your names, e.g.:

    ```C:
        27 - KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: LeonardoFarina DriverEntry\n"));
        61 - KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: LeonardoFarina KmdfHelloWorldEvtDeviceAdd\n"));
    ```

### **Build and Verify**

- To build the project, right-click on "Solution 'KmdfHelloWorld'(1 of 1 project)".
- Click on "Build Solution".
- You should see a "Build succeeded" message in the lower-left corner.

### **Target Virtual Machine Configuration**

- Ensure that we are in "enhanced-session-mode", follow the guide [Share devices with your virtual machine](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/user-guide/enhanced-session-mode).
- Run the command "bcdedit /set testsigning on" in an elevated terminal on the target virtual machine.
- Set up the target virtual machine to connect with Visual Studio:
    - On the target virtual machine, run the MSI installer. You can find the MSI in the Windows Driver Kit (WDK) installation directory.
        - Example: "C:\Program Files (x86)\Windows Kits\10\Remote\x64\WDK Test Target Setup x64-x64_en-us.msi"
- The target virtual machine should be able to ping the host machine.
    - There may be two types of issues when connecting: the Firewall blocking the connection or the network adapter not functioning as expected.
        - Firewall 
            - On the target virtual machine, in Control Panel, go to Network and Internet > Network and Sharing Center. Note your active network. This will be a public, private, or domain network.
            - On the target virtual machine, in Control Panel, go to System and Security > Windows Firewall > Advanced settings > Inbound Rules.
            - In the list of inbound rules, find all Network Discovery rules for your active network. (For example, find all Network Discovery rules that have a Private Profile). Double-click each rule and open the Scope tab. Select Any IP address.
            - In the list of inbound rules, find all File and Printer Sharing rules for your active network. For each of those rules, double-click the rule and open the Scope tab. Select Any IP address.
            - You can look for the following inbound and outbound rules, enable them, and change their scope to 'Any' IP address:
                - Core Networking Diagnostics - ICMP Echo Request (ICMPv6-In)
                - Core Networking Diagnostics - ICMP Echo Request (ICMPv4-In)
            - You can also look for rules containing ICMPv6/ICMPv4 that are linked to printers, enable them, and change their scope to 'Any' IP address.
        - Network Adapter
            - Create a new network switch.
            - You can try different tests by changing the network adapter to external/internal/private.

### **Connect to the Target Virtual Machine**

- Configure our driver extension in Visual Studio on our host machine.
- In Visual Studio:
    - Click on "Extensions".
    - Click on "Driver".
    - Click on "Test".
    - Click on "Configure Devices".
    - Click on "Add New Device".
- Name it "TestComputer".
- In "Network Host Name", enter the IP of our target virtual machine.
- Click "Next".
- In "Port number", enter "50000".
- In "Key", enter "1.2.3.4".
- Click "Next".
- Now you should see requirements being installed on our target virtual machine.
- Complete the configuration.

### **Configure Deployment**

- In the "Solution Explorer", right-click on our KmdfHelloWorld project, click on "Properties".
- Click on "Driver Install", then "Deployment".
- Select "TestComputer" as the "Target Device Name".
- Check "Remove previous driver...".
- Check "Hardware ID Driver Update" and enter "Root\KmdfHelloWorld".
- Click "Apply".
- Now in the "Solution Explorer", right-click on "Solution 'KmdfHelloWorld'(1 of 1 project)", and click on "Deploy Solution".
- You should see a "Deploy succeeded" message in the lower-left corner.

### **Driver Installation**

- On the target virtual machine, run the following commands in an elevated terminal:

    ```powershell
        cd C:\DriverTest\Drivers

        C:\DriverTest\devcon install KmdfHelloWorld.inf root\kmdfhelloworld
    ```

- This should return the message "Drivers installed successfully.".

### **Disable the Driver**

- On the target virtual machine, open the Windows "Device Manager".
- Find "KmdfHelloWorld Device", right-click and click on "Disable device".
- Run the following command in an elevated terminal:
    ```powershell
        sc query KmdfHelloWorld
    ```

- This should return a message that the "KmdfHelloWorld" driver is stopped.
    

### **Connect WinDBG**

- Open WinDBG on our host machine.
- Click on "File".
- Click on "Start debugging".
- Click on "Attach to kernel".
- Click on "Net".
- Fill in the fields with:
    - In "Port number", enter "50000".
    - In "Key", enter "1.2.3.4".
    - In "Target IP", enter the IP of our target machine.
- Now you should see "Debuggee is running..".
- Click on "Break".
- Run the following commands in kd>
    ```powershell
        lm
        .reload
        lmDvmKmdfHelloWorld
        x /D /f KmdfHelloWorld!d*
    ```
- Click on "KmdfHelloWorld!DriverEntry".
- Click on "C:\...\KmdfHelloWorld\Driver.c".
- Right-click on the line where you see your name, click on "Insert Breakpoint" or you can use the left gray panel to set the breakpoint.
- Right-click on the line where you see your name, click on "Run to Cursor".

### **Enable the Driver**

- On the target virtual machine, Run the following command in an elevated terminal:
    ```powershell
        sc start KmdfHelloWorld            
    ```

- This should freeze the target virtual machine.

### **View Name**

- Return to WinDBG.
- Click on "View", then "Memory".
- Now look for your name on the right side.
- Example:
    ```powershell
        KmdfHelloWorld: 
        LeonardoFarina 
        DriverEntry
    ```
- End of the exercise.

### **Resources**
- [Write a Hello World Windows Driver (KMDF)](https://learn.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/writing-a-very-small-kmdf--driver)
- [Provision a computer for driver deployment and testing (WDK 10)](https://learn.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/provision-a-target-computer-wdk-8-1)
- [Share devices with your virtual machine](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/user-guide/enhanced-session-mode)