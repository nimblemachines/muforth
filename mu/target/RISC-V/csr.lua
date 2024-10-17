-- This file is part of muforth: https://muforth.dev/

-- Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

-- Code to process CSR names from a PDF into muforth code.

scraped = {

-- CSR names scraped from riscv-privileged-v1.9.1.pdf
-- We are considering this version to be essentially OBSOLETE. Use a later
-- version instead.
v1_9_1 = [[
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
]],

v1_10 = [[
-- User-level CSRs

-- User Trap Setup
0x000 URW ustatus User status register.
0x004 URW uie User interrupt-enable register.
0x005 URW utvec User trap handler base address.
-- User Trap Handling
0x040 URW uscratch Scratch register for user trap handlers.
0x041 URW uepc User exception program counter.
0x042 URW ucause User trap cause.
0x043 URW utval User bad address or instruction.
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
0x306 MRW mcounteren Machine counter enable.
-- Machine Trap Handling
0x340 MRW mscratch Scratch register for machine trap handlers.
0x341 MRW mepc Machine exception program counter.
0x342 MRW mcause Machine trap cause.
0x343 MRW mtval Machine bad address or instruction.
0x344 MRW mip Machine interrupt pending.
-- Machine Protection and Translation
0x3A0 MRW pmpcfg0 Physical memory protection configuration.
0x3A1 MRW pmpcfg1 Physical memory protection configuration, RV32 only.
0x3A2 MRW pmpcfg2 Physical memory protection configuration.
0x3A3 MRW pmpcfg3 Physical memory protection configuration, RV32 only.
0x3B0 MRW pmpaddr0 Physical memory protection address register.
0x3B1 MRW pmpaddr1 Physical memory protection address register.
0x3BF MRW pmpaddr15 Physical memory protection address register.

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
]],

v1_11 = [[
-- User-level CSRs

-- User Trap Setup
0x000 URW ustatus User status register.
0x004 URW uie User interrupt-enable register.
0x005 URW utvec User trap handler base address.
-- User Trap Handling
0x040 URW uscratch Scratch register for user trap handlers.
0x041 URW uepc User exception program counter.
0x042 URW ucause User trap cause.
0x043 URW utval User bad address or instruction.
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
0x306 MRW mcounteren Machine counter enable.
-- Machine Trap Handling
0x340 MRW mscratch Scratch register for machine trap handlers.
0x341 MRW mepc Machine exception program counter.
0x342 MRW mcause Machine trap cause.
0x343 MRW mtval Machine bad address or instruction.
0x344 MRW mip Machine interrupt pending.
-- Machine Memory Protection
0x3A0 MRW pmpcfg0 Physical memory protection configuration.
0x3A1 MRW pmpcfg1 Physical memory protection configuration, RV32 only.
0x3A2 MRW pmpcfg2 Physical memory protection configuration.
0x3A3 MRW pmpcfg3 Physical memory protection configuration, RV32 only.
0x3B0 MRW pmpaddr0 Physical memory protection address register.
0x3B1 MRW pmpaddr1 Physical memory protection address register.
0x3BF MRW pmpaddr15 Physical memory protection address register.

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
0x320 MRW mcountinhibit Machine counter-inhibit register.
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
0x7B2 DRW dscratch0 Debug scratch register 0.
0x7B3 DRW dscratch1 Debug scratch register 1.
]],

v20211203 = [[
-- User-level CSRs

-- Unprivileged Floating-Point CSRs
0x001 URW fflags Floating-Point Accrued Exceptions.
0x002 URW frm Floating-Point Dynamic Rounding Mode.
0x003 URW fcsr Floating-Point Control and Status Register (frm + fflags).
-- Unprivileged Counter/Timers
0xC00 URO cycle Cycle counter for RDCYCLE instruction.
0xC01 URO time Timer for RDTIME instruction.
0xC02 URO instret Instructions-retired counter for RDINSTRET instruction.
0xC03 URO hpmcounter3 Performance-monitoring counter.
0xC04 URO hpmcounter4 Performance-monitoring counter.
0xC1F URO hpmcounter31 Performance-monitoring counter.
0xC80 URO cycleh Upper 32 bits of cycle, RV32 only.
0xC81 URO timeh Upper 32 bits of time, RV32 only.
0xC82 URO instreth Upper 32 bits of instret, RV32 only.
0xC83 URO hpmcounter3h Upper 32 bits of hpmcounter3, RV32 only.
0xC84 URO hpmcounter4h Upper 32 bits of hpmcounter4, RV32 only.
0xC9F URO hpmcounter31h Upper 32 bits of hpmcounter31, RV32 only.

-- Machine-level CSRs

-- Machine Information Registers
0xF11 MRO mvendorid Vendor ID.
0xF12 MRO marchid Architecture ID.
0xF13 MRO mimpid Implementation ID.
0xF14 MRO mhartid Hardware thread ID.
0xF15 MRO mconfigptr Pointer to configuration data structure.
-- Machine Trap Setup
0x300 MRW mstatus Machine status register.
0x301 MRW misa ISA and extensions
0x302 MRW medeleg Machine exception delegation register.
0x303 MRW mideleg Machine interrupt delegation register.
0x304 MRW mie Machine interrupt-enable register.
0x305 MRW mtvec Machine trap-handler base address.
0x306 MRW mcounteren Machine counter enable.
0x310 MRW mstatush Additional machine status register, RV32 only.
-- Machine Trap Handling
0x340 MRW mscratch Scratch register for machine trap handlers.
0x341 MRW mepc Machine exception program counter.
0x342 MRW mcause Machine trap cause.
0x343 MRW mtval Machine bad address or instruction.
0x344 MRW mip Machine interrupt pending.
0x34A MRW mtinst Machine trap instruction (transformed).
0x34B MRW mtval2 Machine bad guest physical address.

-- Machine Configuration
0x30A MRW menvcfg Machine environment configuration register.
0x31A MRW menvcfgh Additional machine env. conf. register, RV32 only.
0x747 MRW mseccfg Machine security configuration register.
0x757 MRW mseccfgh Additional machine security conf. register, RV32 only.
-- Machine Memory Protection
0x3A0 MRW pmpcfg0 Physical memory protection configuration.
0x3A1 MRW pmpcfg1 Physical memory protection configuration, RV32 only.
0x3A2 MRW pmpcfg2 Physical memory protection configuration.
0x3A3 MRW pmpcfg3 Physical memory protection configuration, RV32 only.
0x3AE MRW pmpcfg14 Physical memory protection configuration.
0x3AF MRW pmpcfg15 Physical memory protection configuration, RV32 only.
0x3B0 MRW pmpaddr0 Physical memory protection address register.
0x3B1 MRW pmpaddr1 Physical memory protection address register.
0x3EF MRW pmpaddr63 Physical memory protection address register.

-- Machine Counter/Timers
0xB00 MRW mcycle Machine cycle counter.
0xB02 MRW minstret Machine instructions-retired counter.
0xB03 MRW mhpmcounter3 Machine performance-monitoring counter.
0xB04 MRW mhpmcounter4 Machine performance-monitoring counter.
0xB1F MRW mhpmcounter31 Machine performance-monitoring counter.
0xB80 MRW mcycleh Upper 32 bits of mcycle, RV32 only.
0xB82 MRW minstreth Upper 32 bits of minstret, RV32 only.
0xB83 MRW mhpmcounter3h Upper 32 bits of mhpmcounter3, RV32 only.
0xB84 MRW mhpmcounter4h Upper 32 bits of mhpmcounter4, RV32 only.
0xB9F MRW mhpmcounter31h Upper 32 bits of mhpmcounter31, RV32 only.
-- Machine Counter Setup
0x320 MRW mcountinhibit Machine counter-inhibit register.
0x323 MRW mhpmevent3 Machine performance-monitoring event selector.
0x324 MRW mhpmevent4 Machine performance-monitoring event selector.
0x33F MRW mhpmevent31 Machine performance-monitoring event selector.

-- Debug/Trace Registers (shared with Debug Mode)
0x7A0 MRW tselect Debug/Trace trigger register select.
0x7A1 MRW tdata1 First Debug/Trace trigger data register.
0x7A2 MRW tdata2 Second Debug/Trace trigger data register.
0x7A3 MRW tdata3 Third Debug/Trace trigger data register.
0x7A8 MRW mcontext Machine-mode context register.
-- Debug Mode Registers
0x7B0 DRW dcsr Debug control and status register.
0x7B1 DRW dpc Debug PC.
0x7B2 DRW dscratch0 Debug scratch register 0.
0x7B3 DRW dscratch1 Debug scratch register 1.
]],
}

scraped_clic = [[
 (NEW) 0xm07 xtvt Trap-handler vector table base address
 (NEW) 0xm45 xnxti Interrupt handler address and enable modifier
 (NEW) 0xm46 xintstatus Current interrupt levels
 (NEW) 0xm47 xintthresh Interrupt-level threshold
 (NEW) 0xm48 xscratchcsw Conditional scratch swap on priv mode change
 (NEW) 0xm49 xscratchcswl Conditional scratch swap on level change
]]

dofile 'target/S08/device/string.lua'

fmt = string.format

function out(s)
    io.write(s .. "\n")
end

function fix_comment(c)
    c = c:gsub("%(", "[")
    c = c:gsub("%)", "]")
    return c
end

function gen_csr(addr, perm, name, comment)
    out(fmt("0%s ( %s) csr %-16s | %s", addr, perm, name, fix_comment(comment)))
end

function line(l, prev_name, prev_num, prev_h)
    if l == "" then return end
    local section = l:match "^%-%- (.+)"
    if section then
        out(fmt("\n( %s)", fix_comment(section)))
        return
    end
    local addr, perm, name, num, h, comment = l:match "0x(%x%x%x) (%u%u%u) (%l+)(%d+)(h?) (.+)"
    if addr then
        if prev_name and name == prev_name and h == prev_h and
            tonumber(num) - tonumber(prev_num) > 1 then

            -- Generate a series of CSRs, from prev_num+1 to num
            addr = tonumber(addr, 16)

            -- Hack! If the name is pmpcfg, we want the comment to end with
            -- ", RV32 only." for odd-numbered registers and "." for
            -- even-numbered ones.
            if name == "pmpcfg" then
                comment = comment:gsub(", RV32 only", "")
                                 :gsub("%.$", "")
            end

            for i = prev_num+1, num do
                -- If CSR is mentioned in the comment, fix the number.
                comment = comment:gsub(name.."%d+", name..i)
                local oddball
                if name == "pmpcfg" then
                    oddball = (i % 2) == 1 and ", RV32 only." or "."
                else
                    oddball = ""
                end
                gen_csr(fmt("%x", addr-num+i), perm, name..i..h, comment..oddball)
            end
            return
        else
            -- Generate a single numbered CSR
            gen_csr(addr:lower(), perm, name..num..h, comment)
            return name, num, h
        end
    end

    -- Normal case.
    local addr, perm, name, comment = l:match "0x(%x%x%x) (%u%u%u) (%l+) (.+)"
    if addr then
        gen_csr(addr:lower(), perm, name, comment)
    end
end

function gen(version, s)
    out(fmt([[
( This file is part of muforth: https://muforth.dev/
|
| Copyright 2002-2024 David Frech. (Read the LICENSE for details.)

( This file contains the definitions of RISC-V CSRs. It is auto-generated
| from target/RISC-V/csr.lua. Do NOT edit this file. Re-run that file
| instead!
|
| These definitions reflect version %s of the RISC-V Privileged
| Architecture Specification (riscv-privileged-%s.pdf)

( Let's define names for the well-known CSRs. This will help both with
| writing code - as these are available to the assembler and compiler - and
| also with disassembling code - as instructions that modify CSRs will be
| able to look up the name by searching the .csr. dictionary chain.)

hex]], version, version))

    local prev_name, prev_num, prev_h
    for l in s:lines() do
        prev_name, prev_num, prev_h = line(l, prev_name, prev_num, prev_h)
    end
end

function gen_clic(version, s)
    out [[
( This file is part of muforth: https://muforth.dev/
|
| Copyright 2002-2024 David Frech. (Read the LICENSE for details.)

( This file contains the definitions of RISC-V CSRs. It is auto-generated
| from target/RISC-V/csr.lua. Do NOT edit this file. Re-run that file
| instead!
|
| These definitions are for use with the CLIC - the RISC-V Core Local
| Interrupt Controller.)

hex

( CLIC CSRs)]]

    for l in s:lines() do
        local addr, name, comment = l:match "0xm(%x%x) x(%l+) (.+)"
        if addr then
            gen_csr("3"..addr, "MRW", "m"..name, comment)
        end
    end
end

function to_file(version, generator, ...)
    local f = io.open("target/RISC-V/csr-"..version..".mu4", "w")
    io.output(f)
    generator(version, ...)
    f:close()
    io.output(io.stdout)
end

to_file("v1.10",     gen, scraped.v1_10)
to_file("v1.11",     gen, scraped.v1_11)
to_file("20211203",  gen, scraped.v20211203)
to_file("clic",      gen_clic, scraped_clic)

