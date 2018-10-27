-- Like we did with the S08, try to generate some of the equates
-- automatically. Or at least automatically-ish.

-- Scraped from MSP430FR6989 datasheet, pp135--151.
fr6989_regs = [[
=0340 Timer_A TA0
TA0 control TA0CTL 00h
Capture/compare control 0 TA0CCTL0 02h
Capture/compare control 1 TA0CCTL1 04h
Capture/compare control 2 TA0CCTL2 06h
Capture/compare control 3 TA0CCTL3 08h
Capture/compare control 4 TA0CCTL4 0Ah
TA0 counter TA0R 10h
Capture/compare 0 TA0CCR0 12h
Capture/compare 1 TA0CCR1 14h
Capture/compare 2 TA0CCR2 16h
Capture/compare 3 TA0CCR3 18h
Capture/compare 4 TA0CCR4 1Ah
TA0 expansion 0 TA0EX0 20h
TA0 interrupt vector TA0IV 2Eh

=0380 Timer_A TA1
TA1 control TA1CTL 00h
Capture/compare control 0 TA1CCTL0 02h
Capture/compare control 1 TA1CCTL1 04h
Capture/compare control 2 TA1CCTL2 06h
TA1 counter TA1R 10h
Capture/compare 0 TA1CCR0 12h
Capture/compare 1 TA1CCR1 14h
Capture/compare 2 TA1CCR2 16h
TA1 expansion 0 TA1EX0 20h
TA1 interrupt vector TA1IV 2Eh

=03c0 Timer_B TB0
TB0 control TB0CTL 00h
Capture/compare control 0 TB0CCTL0 02h
Capture/compare control 1 TB0CCTL1 04h
Capture/compare control 2 TB0CCTL2 06h
Capture/compare control 3 TB0CCTL3 08h
Capture/compare control 4 TB0CCTL4 0Ah
Capture/compare control 5 TB0CCTL5 0Ch
Capture/compare control 6 TB0CCTL6 0Eh
TB0 counter TB0R 10h
Capture/compare 0 TB0CCR0 12h
Capture/compare 1 TB0CCR1 14h
Capture/compare 2 TB0CCR2 16h
Capture/compare 3 TB0CCR3 18h
Capture/compare 4 TB0CCR4 1Ah
Capture/compare 5 TB0CCR5 1Ch
Capture/compare 6 TB0CCR6 1Eh
TB0 expansion 0 TB0EX0 20h
TB0 interrupt vector TB0IV 2Eh

=0400 Timer_A TA2
TA2 control TA2CTL 00h
Capture/compare control 0 TA2CCTL0 02h
Capture/compare control 1 TA2CCTL1 04h
TA2 counter TA2R 10h
Capture/compare 0 TA2CCR0 12h
Capture/compare 1 TA2CCR1 14h
TA2 expansion 0 TA2EX0 20h
TA2 interrupt vector TA2IV 2Eh

=0430 Capacitive Touch I/O 0 Registers
Capacitive Touch I/O 0 control CAPTIO0CTL 0Eh

=0440 Timer_A TA3
TA3 control TA3CTL 00h
Capture/compare control 0 TA3CCTL0 02h
Capture/compare control 1 TA3CCTL1 04h
Capture/compare control 2 TA3CCTL2 06h
Capture/compare control 3 TA3CCTL3 08h
Capture/compare control 4 TA3CCTL4 0Ah
TA3 counter TA3R 10h
Capture/compare 0 TA3CCR0 12h
Capture/compare 1 TA3CCR1 14h
Capture/compare 2 TA3CCR2 16h
Capture/compare 3 TA3CCR3 18h
Capture/compare 4 TA3CCR4 1Ah
TA3 expansion 0 TA3EX0 20h
TA3 interrupt vector TA3IV 2Eh

=0470 Capacitive Touch I/O 1 Registers
Capacitive Touch I/O 1 control CAPTIO1CTL 0Eh

=04a0 RTC_C real-time clock
RTC control 0 RTCCTL0 00h
RTC password RTCPWD 01h
RTC control 1 RTCCTL1 02h
RTC control 3 RTCCTL3 03h
RTC offset calibration RTCOCAL 04h
RTC temperature compensation RTCTCMP 06h
RTC prescaler 0 control RTCPS0CTL 08h
RTC prescaler 1 control RTCPS1CTL 0Ah
RTC prescaler 0 RTCPS0 0Ch
RTC prescaler 1 RTCPS1 0Dh
RTC interrupt vector word RTCIV 0Eh
RTC seconds/counter 1 RTCSEC/RTCNT1 10h
RTC minutes/counter 2 RTCMIN/RTCNT2 11h
RTC hours/counter 3 RTCHOUR/RTCNT3 12h
RTC day of week/counter 4 RTCDOW/RTCNT4 13h
RTC days RTCDAY 14h
RTC month RTCMON 15h
RTC year RTCYEAR 16h
RTC alarm minutes RTCAMIN 18h
RTC alarm hours RTCAHOUR 19h
RTC alarm day of week RTCADOW 1Ah
RTC alarm days RTCADAY 1Bh
Binary-to-BCD conversion BIN2BCD 1Ch
BCD-to-Binary conversion BCD2BIN 1Eh

=04c0 32-bit hardware multiplier
16-bit operand 1 – multiply MPY 00h
16-bit operand 1 – signed multiply MPYS 02h
16-bit operand 1 – multiply accumulate MAC 04h
16-bit operand 1 – signed multiply accumulate MACS 06h
16-bit operand 2 OP2 08h
16 × 16 result low word RESLO 0Ah
16 × 16 result high word RESHI 0Ch
16 × 16 sum extension SUMEXT 0Eh
32-bit operand 1 – multiply low word MPY32L 10h
32-bit operand 1 – multiply high word MPY32H 12h
32-bit operand 1 – signed multiply low word MPYS32L 14h
32-bit operand 1 – signed multiply high word MPYS32H 16h
32-bit operand 1 – multiply accumulate low word MAC32L 18h
32-bit operand 1 – multiply accumulate high word MAC32H 1Ah
32-bit operand 1 – signed multiply accumulate low word MACS32L 1Ch
32-bit operand 1 – signed multiply accumulate high word MACS32H 1Eh
32-bit operand 2 – low word OP2L 20h
32-bit operand 2 – high word OP2H 22h
32 × 32 result 0 – least significant word RES0 24h
32 × 32 result 1 RES1 26h
32 × 32 result 2 RES2 28h
32 × 32 result 3 – most significant word RES3 2Ah
MPY32 control 0 MPY32CTL0 2Ch

=0500 DMA general control
DMA module control 0 DMACTL0 00h
DMA module control 1 DMACTL1 02h
DMA module control 2 DMACTL2 04h
DMA module control 3 DMACTL3 06h
DMA module control 4 DMACTL4 08h
DMA interrupt vector DMAIV 0Eh

=0510 DMA channel 0
DMA channel 0 control DMA0CTL 00h
DMA channel 0 source address low DMA0SAL 02h
DMA channel 0 source address high DMA0SAH 04h
DMA channel 0 destination address low DMA0DAL 06h
DMA channel 0 destination address high DMA0DAH 08h
DMA channel 0 transfer size DMA0SZ 0Ah

=0520 DMA channel 1
DMA channel 1 control DMA1CTL 00h
DMA channel 1 source address low DMA1SAL 02h
DMA channel 1 source address high DMA1SAH 04h
DMA channel 1 destination address low DMA1DAL 06h
DMA channel 1 destination address high DMA1DAH 08h
DMA channel 1 transfer size DMA1SZ 0Ah

=0530 DMA channel 2
DMA channel 2 control DMA2CTL 00h
DMA channel 2 source address low DMA2SAL 02h
DMA channel 2 source address high DMA2SAH 04h
DMA channel 2 destination address low DMA2DAL 06h
DMA channel 2 destination address high DMA2DAH 08h
DMA channel 2 transfer size DMA2SZ 0Ah

=05a0 Memory protection unit
MPU control 0 MPUCTL0 00h
MPU control 1 MPUCTL1 02h
MPU segmentation border 2 MPUSEGB2 04h
MPU segmentation border 1 MPUSEGB1 06h
MPU access management MPUSAM 08h
MPU IP control 0 MPUIPC0 0Ah
MPU IP encapsulation segment border 2 MPUIPSEGB2 0Ch
MPU IP encapsulation segment border 1 MPUIPSEGB1 0Eh

=0640 USCI_B0 I2C/SPI
eUSCI_B control word 0 UCB0CTLW0 00h
eUSCI_B control word 1 UCB0CTLW1 02h
eUSCI_B bit rate 0 UCB0BR0 06h
eUSCI_B bit rate 1 UCB0BR1 07h
eUSCI_B status word UCB0STATW 08h
eUSCI_B byte counter threshold UCB0TBCNT 0Ah
eUSCI_B receive buffer UCB0RXBUF 0Ch
eUSCI_B transmit buffer UCB0TXBUF 0Eh
eUSCI_B I2C own address 0 UCB0I2COA0 14h
eUSCI_B I2C own address 1 UCB0I2COA1 16h
eUSCI_B I2C own address 2 UCB0I2COA2 18h
eUSCI_B I2C own address 3 UCB0I2COA3 1Ah
eUSCI_B received address UCB0ADDRX 1Ch
eUSCI_B address mask UCB0ADDMASK 1Eh
eUSCI_B I2C slave address UCB0I2CSA 20h
eUSCI_B interrupt enable UCB0IE 2Ah
eUSCI_B interrupt flags UCB0IFG 2Ch
eUSCI_B interrupt vector word UCB0IV 2Eh

=0680 USCI_B1 I2C/SPI
eUSCI_B control word 0 UCB1CTLW0 00h
eUSCI_B control word 1 UCB1CTLW1 02h
eUSCI_B bit rate 0 UCB1BR0 06h
eUSCI_B bit rate 1 UCB1BR1 07h
eUSCI_B status word UCB1STATW 08h
eUSCI_B byte counter threshold UCB1TBCNT 0Ah
eUSCI_B receive buffer UCB1RXBUF 0Ch
eUSCI_B transmit buffer UCB1TXBUF 0Eh
eUSCI_B I2C own address 0 UCB1I2COA0 14h
eUSCI_B I2C own address 1 UCB1I2COA1 16h
eUSCI_B I2C own address 2 UCB1I2COA2 18h
eUSCI_B I2C own address 3 UCB1I2COA3 1Ah
eUSCI_B received address UCB1ADDRX 1Ch
eUSCI_B address mask UCB1ADDMASK 1Eh
eUSCI_B I2C slave address UCB1I2CSA 20h
eUSCI_B interrupt enable UCB1IE 2Ah
eUSCI_B interrupt flags UCB1IFG 2Ch
eUSCI_B interrupt vector word UCB1IV 2Eh

=0800 ADC12_B
ADC12_B control 0 ADC12CTL0 00h
ADC12_B control 1 ADC12CTL1 02h
ADC12_B control 2 ADC12CTL2 04h
ADC12_B control 3 ADC12CTL3 06h
ADC12_B window comparator low threshold ADC12LO 08h
ADC12_B window comparator high threshold ADC12HI 0Ah
ADC12_B interrupt flag 0 ADC12IFGR0 0Ch
ADC12_B Interrupt flag 1 ADC12IFGR1 0Eh
ADC12_B interrupt flag 2 ADC12IFGR2 10h
ADC12_B interrupt enable 0 ADC12IER0 12h
ADC12_B interrupt enable 1 ADC12IER1 14h
ADC12_B interrupt enable 2 ADC12IER2 16h
ADC12_B interrupt vector ADC12IV 18h

ADC12_B memory control 0 ADC12MCTL0 20h
ADC12_B memory control 1 ADC12MCTL1 22h
ADC12_B memory control 2 ADC12MCTL2 24h
ADC12_B memory control 3 ADC12MCTL3 26h
ADC12_B memory control 31 ADC12MCTL31 5Eh

ADC12_B memory 0 ADC12MEM0 60h
ADC12_B memory 1 ADC12MEM1 62h
ADC12_B memory 2 ADC12MEM2 64h
ADC12_B memory 3 ADC12MEM3 66h
ADC12_B memory 31 ADC12MEM31 9Eh

=08c0 Comparator_E
Comparator control 0 CECTL0 00h
Comparator control 1 CECTL1 02h
Comparator control 2 CECTL2 04h
Comparator control 3 CECTL3 06h
Comparator interrupt CEINT 0Ch
Comparator interrupt vector word CEIV 0Eh

=0980 CRC32
CRC32 data input CRC32DIW0 00h
Reserved 02h
Reserved 04h
CRC32 data input reverse CRC32DIRBW0 06h
CRC32 initialization and result word 0 CRC32INIRESW0 08h
CRC32 initialization and result word 1 CRC32INIRESW1 0Ah
CRC32 result reverse word 1 CRC32RESRW1 0Ch
CRC32 result reverse word 0 CRC32RESRW1 0Eh
CRC16 data input CRC16DIW0 10h
Reserved 12h
Reserved 14h
CRC16 data input reverse CRC16DIRBW0 16h
CRC16 initialization and result word 0 CRC16INIRESW0 18h
Reserved 1Ah
Reserved 1Ch
CRC16 result reverse word 0 CRC16RESRW1 1Eh

=09c0 AES accelerator
AES accelerator control 0 AESACTL0 00h
AES accelerator control 1 AESACTL1 02h
AES accelerator status AESASTAT 04h
AES accelerator key AESAKEY 06h
AES accelerator data in AESADIN 008h
AES accelerator data out AESADOUT 00Ah
AES accelerator XORed data in AESAXDIN 00Ch
AES accelerator XORed data in (no trigger) AESAXIN 00Eh
]]

dofile 'target/HC08/device/string.lua'

function show_regs(regs)
    local drop = ""
    for l in regs:lines() do
        if l ~= "" then
            local addr, peripheral = l:match "^=(%x%x%x%x) (..-)$"
            if addr then
                print(fmt("%s( %s)\n%s", drop, peripheral, addr))
                drop = "drop\n\n"
            else
                local comment, reg, offset = l:match "^(..-) (%w+) (%x+)h$"
                if comment then
                    local padding = math.max(16 - reg:len(), 3)
                    print(fmt("%s preg %s%s| %s",
                        offset:lower(), reg, (" "):rep(padding), comment))
                end
            end
        end
    end
    print "drop\n"
end

function show_part(name, vecs, regs)
    print (string.format(
        "( Equates for MSP430%s. Extracted from the datasheet using Lua!)\n", name))
    --show_vectors(vecs)
    show_regs(regs)
    print (string.format(
        "( End of auto-generated equates for MSP430%s.)\n", name))
end

show_part("FR6989", nil, fr6989_regs)
