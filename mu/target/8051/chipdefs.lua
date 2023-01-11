-- Core equates scraped from MCS-51 User Manual, 1994 edition.
-- Couldn't grab all 3 columns at once, plus there are weird OCR artifacts:
-- in some of the hex addresses, O rather than 0 appears.

sfr_name_list = [[
*ACC
*B
*PSW
SP
DPL
DPH
*PO
*P1
*P2
*P3
*IP
*IE
TMOD
*TCON
*T2CON
THO
TLO
TH1
TL1
TH2
TL2
RCAP2H
RCAP2L
*SCON
SBUF
PCON
]]

-- columns 2 and 3
sfr_descr_addr_list = [[
Accumulator OEOH
B Register OFOH
Program Status Word ODOH
Stack Pointer 81H
Data Pointer (DPTR) Low Byte 82H
Data Pointer (DPTR) High Byte 83H
Port 0 80H
Port 1 90H
Port 2 OAOH
Port 3 OBOH
Interrupt Priority Control OB8H
Interrupt Enable Control OA8H
Timer/Counter Mode Control 89H
Timer/Counter Control 88H
Timer/Counter 2 Control OC8H
Timer/Counter 0 High Byte 8CH
Timer/Counter 0 Low Byte 8AH
Timer/Counter 1 High Byte 8DH
Timer/Counter 1 Low Byte 8BH
Timer/Counter 2 High Byte OCDH
Timer/Counter 2 Low Byte OCCH
Timer/Counter 2 Capture High Byte OCBH
Timer/Counter 2 Capture Low Byte OCAH
Serial Control 98H
Serial Data Buffer 99H
Power Control 87H 
]]

-- OCR problems: 0 at the end of a register name was read as "O".
function fix_name(d)
    d = d:gsub("O$", "0")
    return d
end

-- OCR problems: leading and trailing 0's in hex numbers were read as "O".
function fix_hex(h)
    h = h:gsub("^O", "")
         :gsub("O", "0")
    return tonumber(h, 16)
end

sfrs = {}
for name in sfr_name_list:gmatch "%*?([%u%d]+)" do
    -- print(name)
    sfrs[#sfrs+1] = { name = fix_name(name) }
end

i = 1
for descr, addr in sfr_descr_addr_list:gmatch "([%w%p ]+) (%w+)H" do
    -- print(descr, fix_hex(addr))
    sfrs[i].addr = fix_hex(addr)
    sfrs[i].descr = descr
    i = i + 1
end

table.sort(sfrs, function(x,y)
    return x.addr < y.addr
end)

for _,s in ipairs(sfrs) do
    print(string.format("%03x equ  %-12s | %s", s.addr, s.name, s.descr))
end
