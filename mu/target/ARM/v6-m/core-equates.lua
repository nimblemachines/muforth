-- ( from the v6-m arch manual:)
v6_scb = [[
0xE000E008 ACTLR The Auxiliary Control Register, ACTLR on page B3-274
0xE000ED00 CPUID CPUID Base Register on page B3-264
0xE000ED04 ICSR Interrupt Control State Register, ICSR on page B3-265
0xE000ED08 VTOR Vector Table Offset Register, VTOR on page B3-267
0xE000ED0C AIRCR Application Interrupt and Reset Control Register, AIRCR on page B3-268
0xE000ED10 SCR Optional System Control Register, SCR on page B3-269
0xE000ED14 CCR Configuration and Control Register, CCR on page B3-271
0xE000ED1C SHPR2 System Handler Priority Register 2, SHPR2 on page B3-272
0xE000ED20 SHPR3 System Handler Priority Register 3, SHPR3 on page B3-273
0xE000ED24 SHCSR System Handler Control and State Register, SHCSR on page C1-329d
0xE000ED30 DFSR Debug Fault Status Register, DFSR on page C1-330d
]]

-- from the v7-m arch manual:
v7_scb = [[
0xE000ED00 CPUID CPUID Base Register on page B3-598.
0xE000ED04 ICSR Interrupt Control and State Register, ICSR on page B3-599.
0xE000ED08 VTOR Vector Table Offset Register, VTOR on page B3-601.
0xE000ED0C AIRCR Application Interrupt and Reset Control Register, AIRCR on page B3-601.
0xE000ED10 SCR System Control Register, SCR on page B3-603.
0xE000ED14 CCR Configuration and Control Register, CCR on page B3-604.
0xE000ED18 SHPR1 System Handler Priority Register 1, SHPR1 on page B3-606.
0xE000ED1C SHPR2 System Handler Priority Register 2, SHPR2 on page B3-606.
0xE000ED20 SHPR3 System Handler Priority Register 3, SHPR3 on page B3-607.
0xE000ED24 SHCSR System Handler Control and State Register, SHCSR on page B3-607.
0xE000ED28 CFSR Configurable Fault Status Register, CFSR on page B3-609. 
0xE000ED2C HFSR HardFault Status Register, HFSR on page B3-612.
0xE000ED30 DFSR Debug Fault Status Register, DFSR on page C1-699.
0xE000ED34 MMFAR MemManage Fault Address Register, MMFAR on page B3-613.
0xE000ED38 BFAR BusFault Address Register, BFAR on page B3-614.
0xE000ED3C AFSR Auxiliary Fault Status Register, AFSR on page B3-614
]]

fmt = string.format

function nicer_hex(h)
    h = h:gsub("(%x+)(%x%x%x%x)", "%1_%2")
         :gsub("(%x%x%x%x)", string.lower)
    return h
end

function nicer_comment(c)
    c = c:gsub(",.*$", "")
         :gsub(" on page.*$", "")
    return c
end

function parse_scb(s)
    local regs = {}

    for addr, name, comment in s:gmatch "0x(%x+) (%S+) (..-)\n" do
        regs[#regs+1] = { name = name, addr = addr, comment = comment }
    end
    return regs
end

function gen_scb()
    v6_regs = parse_scb(v6_scb)
    v7_regs = parse_scb(v7_scb)

    local v6_present = {}
    for _,r in ipairs(v6_regs) do
        v6_present[r.name] = true
    end

    for _,r in ipairs(v7_regs) do
        local prefix = v6_present[r.name] and "    " or "[v7]"
        print(fmt("%s %s equ %-11s | %s",
            prefix,
            nicer_hex(r.addr),
            r.name,
            nicer_comment(r.comment)))
    end
end

gen_scb()
--print(nicer_hex(io.read "a"))
