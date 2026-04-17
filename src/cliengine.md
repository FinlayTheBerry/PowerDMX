PowerDMX API Documentation:
The PowerDMX API is a text based client/server API which allows end users to send commands to PowerCues.
To launch PowerCues in API mode simply run "powercues.exe /c" or "powercues -c".
Once in API mode you can run commands by inputting them to stdin and then get both output and errors from stdout.
Each command is a semicolon : separated list of values where the first value is the command name and the rest are arguments.
Each command must be separated from the next with either an ampersand & or a newline.
Note that both "\n" and "\r\n" are acceptable line endings.
For example if I ran a function foo with the argument bar that returned the string hello world it would look like this:
foo;bar
hello world;
And if instead the foo function returned an error it might look like this:
foo;bar
;Error: foo is not a function.
Return values will end in "\r\n" on Windows and "\n" on Linux.
Here are the possible functions you can invoke:

# EnumDevices
This function doesn't take any arguments and searches for DMX output devices connected to this PC.
It then connects to any devices it finds and stores those connections in an internal list.
Each device is given a unique device ID which is just a positive integer.
These device ID's are guaranteed to be unique until you restart PowerCues.
These device ID's are guaranteed to stay consistent for each device unless it get's disconnected and reconnected.
Device IDs are not always consecuative especially if a device get's disconnected.
Calling EnumDevices again will add any newly connected devices to the internal array and will remove any disconnected devices.
This function returns a comma separated list of device IDs.
Note that when a disconnected device get's removed it's device ID will no longer be contained in the output list and is's entry in the internal list of connections is removed.
That means after a call to EnumDevices you should check if any previously valid device IDs are no longer connected.
Here is an example of calling the EnumDevices function:
EnumDevices
0,1,5;

# SendDmxState
This function takes in the following 3 arguments: the device ID, universe, DMX state.
Note: The device ID must be a valid device ID returned from EnumDevices.
If the device ID is associated with a disconnected device then this function will return an error indicating the device is no longer present.
It will also remove the associated connection from the internal list.
That means any subsequent uses of that device ID will return a generic error instead of a device disconnected error since the internal information about that device was garbage collected.
Note: The universe must be within the range of DMX ports on the device.
Also universes start from 0 so for an ETC Gadget with 2 DMX ports valid universes are 0 or 1.
Note: The DMX state is an array of bytes in the hex format.
Each byte should be represented with 2 hex nibbles and the 0x prefix is not needed.
Additionally inputs must be between 1 and 512 bytes in length.
If the DMX state is less than 512 bytes then it will be padded to the correct length by adding additional 00s to the end.
Here is an example of calling the SendDmxState function:
SendDmxState;0;1;F000FF
;

# GetSerialNumber
This function takes in a device ID and then returns that device's serial number.
Same as above if the device ID is associated with a disconnected device it will be garbage collected.
Here is an example of calling the GetSerialNumber function:
GetSerialNumber;0
123456789;

# GetType
This function takes in a device ID and then returns that device's type.
Same as above if the device ID is associated with a disconnected device it will be garbage collected.
This type will be an integer from the following list:
0 = ETC Gadget
1 = ETC Gadget 2
2 = ETC Gadget 2 CPU2
Here is an example of calling the GetType function:
GetType;0
2;

# Setup
This function doesn't take any arguments and on preforms setup tasks which differ on Windows vs Linux.
On Windows this function installed ETC_WinUsb.inf to your pc so ETC USB devices will work properly.
On Linux this function creates the ETC USB UDEV rule to allow non-root users access to ETC USB devices.
Here is an example of calling the Setup function:
Setup
;

# Exit
This function exits PowerCues and does not return since the process has been killed.
Here is an example of calling the Exit function:
Exit