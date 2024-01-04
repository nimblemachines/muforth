#!/bin/sh

# This file is part of muforth: https://muforth.dev/
#
# Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
#
# Create a udev rules file that matches all the USB devices we care about:
#
#   0403  - FTDI (Future Technology Devices International)
#   0483  - ST-LINK/V2 - for STMicro ARM boards
#   04d8  - Microchip (why? I forget ;-)
#   0d28  - MBED/CMSIS-DAP - for anything supported by mbed
#   15a2  - my Freescale HC08 JS16 board
#   1cbe  - TI/Luminary Stellaris/Tiva ICDI boards
#   2047  - TI MSP-FET, ez-FET, USB BSL, etc
#   28e9  - GigaDevice?
#   2e8a  - Raspberry Pi Pico (PICOBOOT)
#   c251  - Keil CMSIS-DAP - for Freescale Freedom (FRDM) boards
#
# I added the following fake vendor ID and plan to use it for future
# projects:
#
#   f0ff  - Suggests how I feel about the USB-IF's Vendor ID program
#

cat <<EOT
    # udev rules to match USB devices of interest to muforth
EOT

for vid in 0403 0483 04d8 0d28 15a2 1cbe 2047 28e9 2e8a c251 f0ff ; do
    cat <<EOT
    SUBSYSTEMS=="usb", ATTRS{idVendor}=="$vid", OWNER:="$1"
EOT
done
