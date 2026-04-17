#!/bin/sh

# code 1 sudo authentication failure.
# code 127 pkexec authentication failure.
# code 128 udev not installed.
# code 129 no PrivEsc method found.

command -v udevadm >/dev/null 2>&1 || {
    exit 128
}

if [ "$(id -u)" -ne 0 ]; then
    if command -v pkexec >/dev/null 2>&1; then
        exec pkexec "$0" "$@" >/dev/null 2>&1
    elif command -v sudo >/dev/null 2>&1; then
        exec sudo "$0" "$@" 2>/dev/null
    else
        exit 129
    fi
fi

cat << EOF >/usr/lib/udev/rules.d/99-powercues.rules
# ETC Gadget (ETC Gadget 1)
SUBSYSTEM=="usb", ATTR{idVendor}=="14d5", ATTR{idProduct}=="0112", MODE="0666"

# ETC Gadget HS (ETC Gadget 2)
SUBSYSTEM=="usb", ATTR{idVendor}=="14d5", ATTR{idProduct}=="0114", MODE="0666"

# ETC Gadget HS IMX (ETC Gadget 2 CPU 2)
SUBSYSTEM=="usb", ATTR{idVendor}=="14d5", ATTR{idProduct}=="011c", MODE="0666"
EOF
chown +0:+0 /usr/lib/udev/rules.d/99-powercues.rules
chmod 644 /usr/lib/udev/rules.d/99-powercues.rules

udevadm control --reload
udevadm trigger --subsystem-match=usb --attr-match=idVendor=14d5 --attr-match=idProduct=0112
udevadm trigger --subsystem-match=usb --attr-match=idVendor=14d5 --attr-match=idProduct=0114
udevadm trigger --subsystem-match=usb --attr-match=idVendor=14d5 --attr-match=idProduct=011c