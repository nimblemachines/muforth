#!/bin/sh

# Change for your system:
#   openocd is the main openocd directory containing bin/ lib/ etc.
#   libusb_dir is the directory containing a copy of the libusb .so.
openocd=$HOME/hifive1/openocd
libusb_dir=$openocd/lib

riscv=target/RISC-V

[ ! -p $riscv/riscv-gdb ] && mkfifo $riscv/riscv-gdb $riscv/riscv-gdbo

LD_LIBRARY_PATH=$libusb_dir $openocd/bin/openocd -c "gdb_port $riscv/riscv-gdb" -f $riscv/hifive1-openocd.cfg
