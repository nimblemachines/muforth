-- This file is part of muforth: http://muforth.nimblemachines.com/

-- Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)

-- Code to process CSR names from a PDF into muforth code.
-- CSR names scraped from riscv-privileged-v1.9.1.pdf

scraped = [[
-- User-level CSRs

-- User Trap Setup
0x000 URW ustatus User status register.
0x004 URW uie User interrupt-enable register.
0x005 URW utvec User trap handler base address.
-- User Trap Handling
0x040 URW uscratch Scratch register for user trap handlers.
0x041 URW uepc User exception program counter.
0x042 URW ucause User trap cause.
0x043 URW ubadaddr User bad address.
0x044 URW uip User interrupt pending.
-- User Floating-Point CSRs
0x001 URW fflags Floating-Point Accrued Exceptions.
0x002 URW frm Floating-Point Dynamic Rounding Mode.
0x003 URW fcsr Floating-Point Control and Status Register (frm + fflags).
-- User Counter/Timers
0xC00 URO cycle Cycle counter for RDCYCLE instruction.
0xC01 URO time Timer for RDTIME instruction.
0xC02 URO instret Instructions-retired counter for RDINSTRET instruction.
0xC03 URO hpmcounter3 Performance-monitoring counter.
0xC04 URO hpmcounter4 Performance-monitoring counter.
0xC1F URO hpmcounter31 Performance-monitoring counter.
0xC80 URO cycleh Upper 32 bits of cycle, RV32I only.
0xC81 URO timeh Upper 32 bits of time, RV32I only.
0xC82 URO instreth Upper 32 bits of instret, RV32I only.
0xC83 URO hpmcounter3h Upper 32 bits of hpmcounter3, RV32I only.
0xC84 URO hpmcounter4h Upper 32 bits of hpmcounter4, RV32I only.
0xC9F URO hpmcounter31h Upper 32 bits of hpmcounter31, RV32I only.

-- Machine-level CSRs

-- Machine Information Registers
0xF11 MRO mvendorid Vendor ID.
0xF12 MRO marchid Architecture ID.
0xF13 MRO mimpid Implementation ID.
0xF14 MRO mhartid Hardware thread ID.
-- Machine Trap Setup
0x300 MRW mstatus Machine status register.
0x301 MRW misa ISA and extensions
0x302 MRW medeleg Machine exception delegation register.
0x303 MRW mideleg Machine interrupt delegation register.
0x304 MRW mie Machine interrupt-enable register.
0x305 MRW mtvec Machine trap-handler base address.
-- Machine Trap Handling
0x340 MRW mscratch Scratch register for machine trap handlers.
0x341 MRW mepc Machine exception program counter.
0x342 MRW mcause Machine trap cause.
0x343 MRW mbadaddr Machine bad address.
0x344 MRW mip Machine interrupt pending.
-- Machine Protection and Translation
0x380 MRW mbase Base register.
0x381 MRW mbound Bound register.
0x382 MRW mibase Instruction base register.
0x383 MRW mibound Instruction bound register.
0x384 MRW mdbase Data base register.
0x385 MRW mdbound Data bound register.
-- Machine Counter/Timers
0xB00 MRW mcycle Machine cycle counter.
0xB02 MRW minstret Machine instructions-retired counter.
0xB03 MRW mhpmcounter3 Machine performance-monitoring counter.
0xB04 MRW mhpmcounter4 Machine performance-monitoring counter.
0xB1F MRW mhpmcounter31 Machine performance-monitoring counter.
0xB80 MRW mcycleh Upper 32 bits of mcycle, RV32I only.
0xB82 MRW minstreth Upper 32 bits of minstret, RV32I only.
0xB83 MRW mhpmcounter3h Upper 32 bits of mhpmcounter3, RV32I only.
0xB84 MRW mhpmcounter4h Upper 32 bits of mhpmcounter4, RV32I only.
0xB9F MRW mhpmcounter31h Upper 32 bits of mhpmcounter31, RV32I only.
-- Machine Counter Setup
0x320 MRW mucounteren User-mode counter enable.
0x321 MRW mscounteren Supervisor-mode counter enable.
0x322 MRW mhcounteren Hypervisor-mode counter enable.
0x323 MRW mhpmevent3 Machine performance-monitoring event selector.
0x324 MRW mhpmevent4 Machine performance-monitoring event selector.
0x33F MRW mhpmevent31 Machine performance-monitoring event selector.
-- Debug/Trace Registers (shared with Debug Mode)
0x7A0 MRW tselect Debug/Trace trigger register select.
0x7A1 MRW tdata1 First Debug/Trace trigger data register.
0x7A2 MRW tdata2 Second Debug/Trace trigger data register.
0x7A3 MRW tdata3 Third Debug/Trace trigger data register.
-- Debug Mode Registers
0x7B0 DRW dcsr Debug control and status register.
0x7B1 DRW dpc Debug PC.
0x7B2 DRW dscratch Debug scratch register.
]]

dofile 'target/HC08/device/string.lua'

fmt = string.format

function fix_comment(c)
    c = c:gsub("%(", "[")
    c = c:gsub("%)", "]")
    return c
end

function gen_csr(addr, perm, name, comment)
    return fmt("0%s ( %s) csr %s   ( %s)", addr, perm, name, fix_comment(comment))
end

function line(l)
    if l == "" then return end
    local section = l:match "^%-%- (.+)"
    if section then
        print(fmt("\n( %s)", fix_comment(section)))
        return
    end
    if l:match "counter4" or l:match "counter31" then return end
    if l:match "event4" or l:match "event31" then return end
    if l:match "counter3" or l:match "event3" then
        -- Generate a series of CSRs, from 3 to 31
        local addr, perm, base, h, comment
            = l:match "(0x%x%x%x) (%u%u%u) (%l+)%d+(h?) (.+)"
        addr = tonumber(addr)
        for i = 3, 31 do
            print(gen_csr(fmt("%x", addr-3+i), perm, base..i..h, comment))
        end
        return
    end

    -- Normal case.
    local addr, perm, name, comment = l:match "0x(%x%x%x) (%u%u%u) (%w+) (.+)"
    print(gen_csr(addr:lower(), perm, name, comment))
end

function gen(s)
  for l in s:lines() do line(l) end
end

print [[
( This file is part of muforth: http://muforth.nimblemachines.com/

  Copyright 2002-2018 David Frech. (Read the LICENSE for details.)

( This file file contains the definitions of the RISC-V CSRs. It is
  auto-generated from target/RISC-V/csr.lua. Do NOT edit this file. Re-run
  that file instead!)

( Let's define names for the well-known CSRs. This will help both with
  writing code - as these are available to the assembler and compiler - and
  also with disassembling code - as instructions that modify CSRs will be
  able to look up the name by searching the .csr. dictionary chain.)

: csr  ( offset)   current preserve  .csr. definitions  constant ;

hex]]
gen(scraped)
