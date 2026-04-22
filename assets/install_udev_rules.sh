#!/bin/sh -e

command -v udevadm >/dev/null 2>&1 || {
    printf "\033[31mError: Udevadm could not be found. A system with udev is required to run this script.\033[0m\n"
    exit 1
}

if [ "$(id -u)" -ne 0 ]; then
    printf "\033[33mWarning: Root is most likely required to install udev rules. If this fails try sudo ./powercues_udev_setup.sh\033[0m\n"
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

printf "Done! New rules installed to /usr/lib/udev/rules.d/99-powercues.rules and udev reloaded.\n"