# PowerDMX
PowerDMX is an unofficial driver for ETC Gadget USB to DMX adapters.  
PowerDMX is free and open source under the Apache 2.0 license.  
PowerDMX supports Windows, MacOS, and Linux.  
The core of PowerDMX is a standalone C++ binary which can be sent commands via the command line (stdin/stdout).  
PowerDMX also offers a python library which wraps this binary in a python friendly interface while marshaling commands to the same C++ binary internally.

# How To Use
Download [the latest release from GitHub](https://github.com/FinlayTheBerry/PowerDMX/releases/latest) and download the version for your operating system. Extract the contents and you will find the PowerDMX binary along with PowerDMX.py. Place both files alongside your python code and remember to add import PowerDMX. Then you can interact with your ETC Gadget using the provided library. Here is an example of connecting to the device, setting address 1 to full.

PowerDMX.Gadgets[0].SetDmxState(0, 0, b"FF")


# Windows Setup Instructions
This software is supported on all versions of Windows with ucrtbase.dll.  
ucrtbase.dll comes with Windows 10 and newer and can be installed after the fact on Windows Vista and newer.  
To download ucrtbase.dll on older versions of Windows go [here](https://support.microsoft.com/en-us/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c).  
If you don't already have the ETC EOS Console software installed you will need to manually load the ETC WinUSB drivers.  
This is suprisingly easy. Just navigate to `C:\msys64\home\finlaytheberry\PowerDMX\assets` in file explorer then right click on `etc_winusb.inf` and press Install.  

# Windows Build Instructions
To build start by downloading MSYS2 [here](https://www.msys2.org/).  
Then launch the "MSYS2 UCRT64" terminal.  
Run: `pacman -Syu git mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-autotools --noconfirm`.  
Run: `git clone https://github.com/FinlayTheBerry/PowerDMX.git ~/PowerDMX`.  
Run: `cd ~/PowerDMX && make`.  
The build files are `~/PowerDMX/PowerDMX.py` and `~/PowerDMX/bin/PowerDMX.exe`.  

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
The build files are `~/PowerDMX/PowerDMX.py` and `~/PowerDMX/bin/PowerDMX`.  

# Linux Setup Instructions

# Linux Build Instructions