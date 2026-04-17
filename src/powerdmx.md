ETC Gadget, ETC Gadget 2, and ETC Gadget 2 CPU 2 all use the same protocol.

Internally the ETC Gadget is called ETC Gadget.
Internally the ETC Gadget 2 is called ETC Gadget HS.
Internally the ETC Gadget 2 CPU 2 is called ETC Gadget HS IMX.

ETC Vendor ID (VID) = 0x14D5
ETC Gadget Product ID (PID) = 0x0112
ETC Gadget HS Product ID (PID) = 0x0114
ETC Gadget HS IMX Product ID (PID) = 0x011C

The device contains the following strings in the usb configuration:
Index 1 "ETC" as manufacture.
Index 2 "ETC GADGET 2 (CPU2)" as product.
Index 3 "4267B3624C" as Unknown.
Index 4 "HID INTERFACE" as interface 0.
Index 5 "VENDOR BULK INTERFACE" as interface 1.
Index 6 "ETC CONSOLE I/O 2PORT iMXRT" as Unknown.
Index 7 "ETC CONSOLE I/O 4PORT iMXRT" as Unknown.

The device contains only 1 configuration.

The device contains the following 2 interfaces:
Interface 0 HID (0x03) Boot Device (0x01) Mouse (0x02) with 2 endpoints.
Interface 1 Vendor Specific (0xFF) Null (0x00) Null (0x00) with 2 endpoints.

The device contains the following 5 endpoints:
Endpoint 0 is the default endpoint for device setup.
Endpoint 129 for interface 0 accepts interrupt transfers in.
Endpoint 1 for interface 0 accepts interrupt transfers out.
Endpoint 130 for interface 2 accepts bulk transfers in.
Endpoint 2 for interface 1 accepts bulk transfers out.

Send DMX State API:
You can send a DMX state to the ETC Gadget by sending a USB bulk transfer out to endpoint 2.
The sent packet should be 518 bytes in size and match this structure:
struct DmxStatePacket {
	uint8_t magic = 0x20;
	uint8_t universe;
	uint8_t reserved[4] = { 0x00, 0x00, 0x00, 0x02 };
	uint8_t dmxState[512];
};
The magic value tells the ETC Gadget that what's to come is a DMX_STATE_PACKET.
The reserved value must be exactly as shown or the ETC Gadget will ignore the request.
Universe is the 0 based index of the target DMX port on the device. Valid values are 0 or 1.
DmxState is the 512 byte DMX state to send to the lights.
Note that this API doesn't send any response.

Send Control Command API:
You can ask the ETC Gadget to run various commands by sending a USB bulk transfer out to endpoint 2.
The same API can also be called asynchronously by sending a USB interrupt transfer out to endpoint 1.
The sent packet can be a few different sizes (more on that later) but must match this structure:
struct ControlCommandPacket {
	uint8_t magic = 0x00;
	uint8_t command;
	uint8_t reserved[2] = { 0x00, 0x00 };
	uint8_t extraData[ANYSIZEARRAY];
};
The recieved packet can also be a few different sizes but must match this structure:
struct ControlResponsePacket {
	uint8_t magic = 0x00;
	uint8_t command;
	uint8_t reserved[2] = { 0x00, 0x00 };
	uint8_t response[ANYSIZEARRAY];
};
Note that reserved must be 0 otherwise the ETC Gadget will ignore your command.
The minimum size of the sent packet is determinand by which command is being invoked.
Some commands have additional information which must be stored in the extra data array.
If the sent command is larger than needed the ETC Gadget will usually ignore the extra data and run the command anyways.
Sometimes it will get glitchy and once it got into a weird state where it stopped working until I restart it.
Additionally note that the response buffer is not cleared between replies so bits of old data can be left behind at the end if the second response is smaller than the first.
Each of the possible values for command is listed below:

1 = Reset device:
This command causes the ETC Gadget to disconnect from the PC then reconnect.
This resets the ETC Gadget's state completely.
Example output:
None since the device disconnects and reboots.

2 = Get version:
This command returns a version string in the format X.X.X.X
This version string is preceeded by the byte 0x03.
Example output ():
--02----03322E302E302E30 aka "2.0.0.0"

3 = Get SW version:
This command returns another version string which is labelled as SW.
This version string is preceeded by the byte 0x07.
Example output:
--03----07322E322E302E35 aka "2.2.0.5"

4 = Get full version:
This command returns the full version string for all the different systems.
The version string is also preceeded by the byte 0x80.
Example output:
--04----80244761646765742032202843505532293A53575F322E322E302E353A424C5F312E312E302E303A465047415F312E322E31723130 aka "$Gadget 2 (CPU2):SW_2.2.0.5:BL_1.1.0.0:FPGA_1.2.1r10"

8 = Get serial number:
This command returns the serial number as a little endian 32 bit number.
Note that since the response includes the magic, command, and reserved fields the actual data is at offset 4.
Example output:
--08----312C691D--------------------------------

10 = Get three bytes of 0s:
I have no idea what this command does but it returns 3 bytes of all zeros.
Example output:
--08----------

11 = Get BL version:
This command returns another version string which is labelled as BL.
This version string is preceeded by the byte 0x07.
Example output:
--0B----07312E312E302E30 aka "1.1.0.0"

7 = Undocumented:
5 = Undocumented:
6 = Undocumented:
17 = Undocumented:

Notes on ETC Gadget 2 CPU1:
Most of the control commands work exactly the same regardless of which version of the ETC Gadget you have.
However, there are a few small differences worth noting.
Both the "Get version" and "Get full version" commands return strings of different lengths so make sure you locate the null terminator before assuming.
Additionaly the "Get SW version" and "Get BL version" commands work completely different on the ETC Gadget CPU1 vs CPU2.
On the older ETC Gadget 2 the "Get SW version" command takes in a binary value with one byte per version number.
Example output:
--03----06010201090008
And that lines up with what the "Get full version" command gives us which is:
--04----32492F4F2043617264204B696E657469733A53575F312E322E312E392E302E383A49445F37 aka "2I/O Card Kinetis:SW_1.2.1.9.0.8:ID_7"
Keep in mind the preceeding values in front of all strings are also not present on older versions.
And finally the "Get BL version" endpoint is a little glitchy.
It returns this:
--0B----06010200090007
Which seems to almost resemble a version but it's a little weird.

Set Speed Command API:
struct SpeedCommandPacket {
	uint8_t magic = 0x00;
	uint8_t magic2 = 0x20;
	uint8_t universe;
	uint8_t reserved = 0x01;
	uint8_t speed; // 0 = Max, 1 = Fast, 2 = Medium, 3 = Slow
	uint8_t enabled;
}

Only thing left to understand is RDM packets.
RDM_CONFIG_PACKET size 70 or 58.
This packet is largely a mystery.
It will always contain the text "etcrdm".
The magic value for this kind of packet is 0x23.

Note On INF File:
How to get ETC_WinUsb.inf without stealing it.
Download ETC_EosFamily_v3.2.12.1.zip and extract it.
Search inside ETC_EosFamily_v3.2.12.1.exe for ETC_WinUsb.exe.
Search inside ETC_WinUsb.exe for Disk1/data1.cab.
Search inside data1.cab for ETC_WinUsb.inf and ETC_WinUsb.cat.

Notes on timings:
1 second passed from packet 2832 to 2668 which is 164 total packets or 82 DMX states per second.
1 second passed from packet 3000 to 2832 which is 168 total packets or 84 DMX states per second.
1 second passed from packet 3168 to 3000 which is 168 total packets or 84 DMX states per second.
1 second passed from packet 3334 to 3168 which is 166 total packets or 82 DMX states per second.
1 second passed from packet 3500 to 3334 which is 166 total packets or 82 DMX states per second.
So it seems that rougly 80 to 85 packets per second is ideal.