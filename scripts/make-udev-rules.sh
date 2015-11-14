#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2015 David Frech. (Read the LICENSE for details.)

# Create a udev rules file that matches all the USB devices we care about:
#   0483  - ST-LINK/V2 - for STMicro ARM boards
#   04d8  - Microchip (why? I forget ;-)
#   15a2  - my Freescale HC08 JS16 board
#   1cbe  - TI/Luminary Stellaris/Tiva ICDI boards
#   c251  - Keil CMSIS-DAP - for Freescale Freedom (FRDM) boards
#   0d28  - MBED/CMSIS-DAP - for anything supported by mbed

cat <<EOT
    # udev rules to match USB devices of interest to muFORTH
EOT

for vid in 0483 04d8 15a2 1cbe c251 0d28 ; do
    cat <<EOT
    SUBSYSTEMS=="usb", ATTRS{idVendor}=="$vid", OWNER:="$1"
EOT
done
