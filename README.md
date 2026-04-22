# PowerDMX
PowerDMX is an unofficial driver for ETC Gadget USB to DMX adapters.  
PowerDMX is free and open source under the Apache 2.0 license.  
PowerDMX supports Windows, MacOS, and Linux.  
The core of PowerDMX is a standalone C++ binary which can be sent commands via the command line (stdin/stdout).  
PowerDMX also offers a python library which wraps this binary in a python friendly interface while marshaling commands to the same C++ binary internally.  

# How To Use
Download [the latest release from GitHub](https://github.com/FinlayTheBerry/PowerDMX/releases/latest) and download the version for your operating system.  
Extract the contents and you will find the PowerDMX folder. Place this folder next to your `.py` script.  
Remember to add `import PowerDMX`. Then you can interact with your ETC Gadget using the provided library.  
Here is an example of connecting to an ETC Gadget and setting all 512 addresses at full.  

```python
import PowerDMX
numGadgets = PowerDMX.Enum()
if numGadgets < 1:
    raise Exception("No ETC Gadget device found.")
connectionId = PowerDMX.Connect(0)
PowerDMX.SetDmxState(connectionId, 0, [ 255 ] * 512)
PowerDMX.Disconnect(connectionId)
```

# Windows Setup Instructions
This software is supported on all versions of Windows with ucrtbase.dll.  
ucrtbase.dll comes with Windows 10 and newer and can be installed after the fact on Windows Vista and newer.  
To download ucrtbase.dll on older versions of Windows go [here](https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c).  
If you don't already have the ETC EOS Console software installed you will need to manually load the ETC WinUSB drivers.  
This is suprisingly easy. Just right click on `etc_winusb.inf` in file explorer and press Install.  

# Windows Build Instructions
To build start by downloading MSYS2 [here](https://www.msys2.org/).  
Then launch the "MSYS2 UCRT64" terminal.  
Run: `pacman -Syu git mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-autotools --noconfirm`.  
Run: `git clone https://github.com/FinlayTheBerry/PowerDMX.git ~/PowerDMX`.  
Run: `cd ~/PowerDMX && make`.  
The build files are `~/PowerDMX/PowerDMX_PY/*`, `~/PowerDMX/bin/PowerDMX.exe`, `~/PowerDMX/assets/etc_winusb.inf`, and `~/PowerDMX/assets/etc_winusb.cat`.  

# macOS Setup Instructions
This software is supported on all Intel based x86_64 Macs.  
This software should also run on Apple silicone macs through Rosetta 2.  
Very old versions of macOS may not work. If unsupported consider using OpenCore.  
No special setup is required. Just download PowerDMX and run it.

# macOS Build Instructions
First you will need to install install xcode. If your mac is too old consider using OpenCore.  
You will also need several pieces of software which can be easily installed with homebrew.  
Run: `brew install autoconf automake libtool`.  
Run: `git clone https://github.com/FinlayTheBerry/PowerDMX.git ~/PowerDMX`.  
Run: `cd ~/PowerDMX && make`.  
The build files are `~/PowerDMX/PowerDMX_PY/*`, and `~/PowerDMX/bin/PowerDMX`.  

# Linux Setup Instructions
PowerDMX for Linux requires an x86_64 CPU.  
PowerDMX for Linux is a statically linked binary which depends only on the kernel itself.  
If you manage to find a system still running a kernel so old that this software doesn't work then you have bigger fish to fry.  
If you get errors ensure PowerDMX has permission to access files in `/dev/bus/usb/`.  
If you are running on a system with udev run `sudo ./install_udev_rules.sh` to install a udev rule which gives all users access to ETC Gadget devices.  

# Linux Build Instructions
You will need several peices of software to build PowerDMX.  
PowerDMX uses it's own internal copy of musl gcc so the system wide compiler will be ignored.  
PowerDMX requires `make git curl tar nproc uname strip autoconf automake libtool m4 pkg-config` and many other standard C and C++ build tools.  
Run: `git clone https://github.com/FinlayTheBerry/PowerDMX.git ~/PowerDMX`.  
Run: `cd ~/PowerDMX && make`.  
The build files are `~/PowerDMX/PowerDMX_PY/*`, `~/PowerDMX/bin/PowerDMX`, and `~/PowerDMX/assets/install_udev_rules.sh`.  