# PowerDMX
PowerDMX is an unofficial driver for ETC Gadget USB to DMX adapters.  
PowerDMX is free and open source under the Apache 2.0 license.  
PowerDMX supports Windows, MacOS, and Linux.  
The core of PowerDMX is a standalone C++ binary which can be sent commands via the command line (stdin/stdout).  
PowerDMX also offers a python library which wraps this binary in a python friendly interface while marshaling commands to the same C++ binary internally.

# How To Use
Download [the latest release from GitHub](https://github.com/FinlayTheBerry/PowerDMX/releases/latest) and download the version for your operating system. Extract the contents and you will find the PowerDMX binary along with PowerDMX.py. Place both files alongside your python code and remember to add import PowerDMX. Then you can interact with your ETC Gadget using the provided library. Here is an example of connecting to the device, setting address 1 to full.

PowerDMX.Gadgets[0].SetDmxState(0, 0, b"FF")


Download:
Msys2
Open MSYS2 UCRT64
Run pacman -Syu git mingw-w64-ucrt-x86_64-toolchain
Run git clone git@github.com:FinlayTheBerry/PowerDMX.git ./PowerDMX
Run cd ./PowerDMX
Run make
