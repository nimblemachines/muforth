-- EFM8BB1 SFR equates
-- Scraped from EFM8BB1 reference manual:
-- https://www.silabs.com/documents/public/reference-manuals/efm8bb1-rm.pdf

sfr_list = [[
ACC 0xE0 Accumulator
ADC0AC 0xB3 ADC0 Accumulator Configuration
ADC0CF 0xBC ADC0 Configuration
ADC0CN0 0xE8 ADC0 Control 0
ADC0CN1 0xB2 ADC0 Control 1
ADC0GTH 0xC4 ADC0 Greater-Than High Byte
ADC0GTL 0xC3 ADC0 Greater-Than Low Byte
ADC0H 0xBE ADC0 Data Word High Byte
ADC0L 0xBD ADC0 Data Word Low Byte
ADC0LTH 0xC6 ADC0 Less-Than High Byte
ADC0LTL 0xC5 ADC0 Less-Than Low Byte
ADC0MX 0xBB ADC0 Multiplexer Selection
ADC0PWR 0xDF ADC0 Power Control
ADC0TK 0xB9 ADC0 Burst Mode Track Time
B 0xF0 B Register
CKCON0 0x8E Clock Control 0
CLKSEL 0xA9 Clock Select
CMP0CN0 0x9B Comparator 0 Control 0
CMP0MD 0x9D Comparator 0 Mode
CMP0MX 0x9F Comparator 0 Multiplexer Selection
CMP1CN0 0xBF Comparator 1 Control 0
CMP1MD 0xAB Comparator 1 Mode
CMP1MX 0xAA Comparator 1 Multiplexer Selection
CRC0AUTO 0xD2 CRC0 Automatic Control
CRC0CN0 0xCE CRC0 Control 0
CRC0CNT 0xD3 CRC0 Automatic Flash Sector Count
CRC0DAT 0xDE CRC0 Data Output
CRC0FLIP 0xCF CRC0 Bit Flip
CRC0IN 0xDD CRC0 Data Input
DERIVID 0xAD Derivative Identification
DEVICEID 0xB5 Device Identification
DPH 0x83 Data Pointer High
DPL 0x82 Data Pointer Low
EIE1 0xE6 Extended Interrupt Enable 1
EIP1 0xF3 Extended Interrupt Priority 1
FLKEY 0xB7 Flash Lock and Key
HFO0CAL 0xC7 High Frequency Oscillator 0 Calibration
IE 0xA8 Interrupt Enable
IP 0xB8 Interrupt Priority
IT01CF 0xE4 INT0/INT1 Configuration
LFO0CN 0xB1 Low Frequency Oscillator Control
P0 0x80 Port 0 Pin Latch
P0MASK 0xFE Port 0 Mask
P0MAT 0xFD Port 0 Match
P0MDIN 0xF1 Port 0 Input Mode
P0MDOUT 0xA4 Port 0 Output Mode
P0SKIP 0xD4 Port 0 Skip
P1 0x90 Port 1 Pin Latch
P1MASK 0xEE Port 1 Mask
P1MAT 0xED Port 1 Match
P1MDIN 0xF2 Port 1 Input Mode
P1MDOUT 0xA5 Port 1 Output Mode
P1SKIP 0xD5 Port 1 Skip
P2 0xA0 Port 2 Pin Latch
P2MDOUT 0xA6 Port 2 Output Mode
PCA0CENT 0x9E PCA Center Alignment Enable
PCA0CLR 0x9C PCA Comparator Clear Control
PCA0CN0 0xD8 PCA Control
PCA0CPH0 0xFC PCA Channel 0 Capture Module High Byte
PCA0CPH1 0xEA PCA Channel 1 Capture Module High Byte
PCA0CPH2 0xEC PCA Channel 2 Capture Module High Byte
PCA0CPL0 0xFB PCA Channel 0 Capture Module Low Byte
PCA0CPL1 0xE9 PCA Channel 1 Capture Module Low Byte
PCA0CPL2 0xEB PCA Channel 2 Capture Module Low Byte
PCA0CPM0 0xDA PCA Channel 0 Capture/Compare Mode
PCA0CPM1 0xDB PCA Channel 1 Capture/Compare Mode
PCA0CPM2 0xDC PCA Channel 2 Capture/Compare Mode
PCA0H 0xFA PCA Counter/Timer High Byte
PCA0L 0xF9 PCA Counter/Timer Low Byte
PCA0MD 0xD9 PCA Mode
PCA0POL 0x96 PCA Output Polarity
PCA0PWM 0xF7 PCA PWM Configuration
PCON0 0x87 Power Control
PRTDRV 0xF6 Port Drive Strength
PSCTL 0x8F Program Store Control
PSW 0xD0 Program Status Word
REF0CN 0xD1 Voltage Reference Control
REG0CN 0xC9 Voltage Regulator 0 Control
REVID 0xB6 Revision Identifcation
RSTSRC 0xEF Reset Source
SBUF0 0x99 UART0 Serial Port Data Buffer
SCON0 0x98 UART0 Serial Port Control
SMB0ADM 0xD6 SMBus 0 Slave Address Mask
SMB0ADR 0xD7 SMBus 0 Slave Address
SMB0CF 0xC1 SMBus 0 Configuration
SMB0CN0 0xC0 SMBus 0 Control
SMB0DAT 0xC2 SMBus 0 Data
SMB0TC 0xAC SMBus 0 Timing and Pin Control
SP 0x81 Stack Pointer
SPI0CFG 0xA1 SPI0 Configuration
SPI0CKR 0xA2 SPI0 Clock Rate
SPI0CN0 0xF8 SPI0 Control
SPI0DAT 0xA3 SPI0 Data
TCON 0x88 Timer 0/1 Control
TH0 0x8C Timer 0 High Byte
TH1 0x8D Timer 1 High Byte
TL0 0x8A Timer 0 Low Byte
TL1 0x8B Timer 1 Low Byte
TMOD 0x89 Timer 0/1 Mode
TMR2CN0 0xC8 Timer 2 Control 0
TMR2H 0xCD Timer 2 High Byte
TMR2L 0xCC Timer 2 Low Byte
TMR2RLH 0xCB Timer 2 Reload High Byte
TMR2RLL 0xCA Timer 2 Reload Low Byte
TMR3CN0 0x91 Timer 3 Control 0
TMR3H 0x95 Timer 3 High Byte
TMR3L 0x94 Timer 3 Low Byte
TMR3RLH 0x93 Timer 3 Reload High Byte
TMR3RLL 0x92 Timer 3 Reload Low Byte
VDM0CN 0xFF Supply Monitor Control
WDTCN 0x97 Watchdog Timer Control
XBR0 0xE1 Port I/O Crossbar 0
XBR1 0xE2 Port I/O Crossbar 1
XBR2 0xE3 Port I/O Crossbar 2
]]

sfrs = {}
for name, addr, descr in string.gmatch(sfr_list, "([%u%d]+) 0x(%x%x) ([%w%p ]+)") do
    -- print(name, addr, descr)
    sfrs[#sfrs+1] = { name = name, addr = tonumber(addr, 16), descr = descr }
end

table.sort(sfrs, function(x,y)
    return x.addr < y.addr
end)

for _,s in ipairs(sfrs) do
    print(string.format("%03x equ  %-12s | %s", s.addr, s.name, s.descr))
end
