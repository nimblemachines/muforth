#!/bin/sh

openocd=$HOME/hifive1/openocd
riscv=target/RISC-V

[ ! -p $riscv/riscv-gdb ] && mkfifo $riscv/riscv-gdb $riscv/riscv-gdbo

$openocd/bin/openocd -c "gdb_port $riscv/riscv-gdb" -f $riscv/hifive1-openocd.cfg
