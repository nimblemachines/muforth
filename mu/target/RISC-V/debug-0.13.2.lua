-- Scraped from pdf
regs = [[
0x04 Abstract Data 0 (data0) 30
0x0f Abstract Data 11 (data11)
0x10 Debug Module Control (dmcontrol) 22
0x11 Debug Module Status (dmstatus) 20
0x12 Hart Info (hartinfo) 25
0x13 Halt Summary 1 (haltsum1) 31
0x14 Hart Array Window Select (hawindowsel) 26
0x15 Hart Array Window (hawindow) 26
0x16 Abstract Control and Status (abstractcs) 27
0x17 Abstract Command (command) 28
0x18 Abstract Command Autoexec (abstractauto) 29
0x19 Configuration String Pointer 0 (confstrptr0) 29
0x1a Configuration String Pointer 1 (confstrptr1)
0x1b Configuration String Pointer 2 (confstrptr2)
0x1c Configuration String Pointer 3 (confstrptr3)
0x1d Next Debug Module (nextdm) 30
0x20 Program Buffer 0 (progbuf0) 30
0x2f Program Buffer 15 (progbuf15)
0x30 Authentication Data (authdata) 31
0x34 Halt Summary 2 (haltsum2) 32
0x35 Halt Summary 3 (haltsum3) 32
0x37 System Bus Address 127:96 (sbaddress3) 36
0x38 System Bus Access Control and Status (sbcs) 32
0x39 System Bus Address 31:0 (sbaddress0) 34
0x3a System Bus Address 63:32 (sbaddress1) 35
0x3b System Bus Address 95:64 (sbaddress2) 35
0x3c System Bus Data 31:0 (sbdata0) 36
0x3d System Bus Data 63:32 (sbdata1) 37
0x3e System Bus Data 95:64 (sbdata2) 37
0x3f System Bus Data 127:96 (sbdata3) 38
0x40 Halt Summary 0 (haltsum0) 31]]

fmt = string.format

function print_reg(name, addr, descr)
    print(fmt("%02x debugreg %-16s | %s", addr, name, descr))
end

function print_reg_fillin(name, addr, descr)
    local function fillin(first, last)
        local function reindex(s, index)
            s = s:gsub("%d+$", tostring(index), 1)
            return s
        end
        for index = first, last do
            print_reg(reindex(name, index), addr + index, reindex(descr, index))
        end
    end

    print_reg(name, addr, descr)
    if name == "data0" then fillin(1, 10) end
    if name == "progbuf0" then fillin(1, 14) end
end

-- generate equates/constants
for addr, descr, regname in regs:gmatch "0x(%x+)%s+(..-)%s+%((%w+)%)" do
    print_reg_fillin(regname, tonumber(addr, 16), descr)
end
