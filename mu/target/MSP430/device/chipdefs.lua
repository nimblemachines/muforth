-- Like we did with the S08, try to generate some of the equates
-- automatically. Or at least automatically-ish.

-- Scraped from MSP430F5529 datasheet, pp68--81.
-- Copied verbatim table-by-table, then broke up tables 6-25 and 6-37 into
-- subtables, since each needs its own base address.
f5529_regs = [[
Table 6-16. Special Function Registers (Base Address: 0100h)
REGISTER DESCRIPTION REGISTER OFFSET
SFR interrupt enable SFRIE1 00h
SFR interrupt flag SFRIFG1 02h
SFR reset pin control SFRRPCR 04h

Table 6-17. PMM Registers (Base Address: 0120h)
REGISTER DESCRIPTION REGISTER OFFSET
PMM control 0 PMMCTL0 00h
PMM control 1 PMMCTL1 02h
SVS high-side control SVSMHCTL 04h
SVS low-side control SVSMLCTL 06h
PMM interrupt flags PMMIFG 0Ch
PMM interrupt enable PMMIE 0Eh
PMM power mode 5 control PM5CTL0 10h

Table 6-18. Flash Control Registers (Base Address: 0140h)
REGISTER DESCRIPTION REGISTER OFFSET
Flash control 1 FCTL1 00h
Flash control 3 FCTL3 04h
Flash control 4 FCTL4 06h

Table 6-19. CRC16 Registers (Base Address: 0150h)
REGISTER DESCRIPTION REGISTER OFFSET
CRC data input CRC16DI 00h
CRC data input reverse byte CRCDIRB 02h
CRC initialization and result CRCINIRES 04h
CRC result reverse byte CRCRESR 06h

Table 6-20. RAM Control Registers (Base Address: 0158h)
REGISTER DESCRIPTION REGISTER OFFSET
RAM control 0 RCCTL0 00h

Table 6-21. Watchdog Registers (Base Address: 015Ch)
REGISTER DESCRIPTION REGISTER OFFSET
Watchdog timer control WDTCTL 00h

Table 6-22. UCS Registers (Base Address: 0160h)
REGISTER DESCRIPTION REGISTER OFFSET
UCS control 0 UCSCTL0 00h
UCS control 1 UCSCTL1 02h
UCS control 2 UCSCTL2 04h
UCS control 3 UCSCTL3 06h
UCS control 4 UCSCTL4 08h
UCS control 5 UCSCTL5 0Ah
UCS control 6 UCSCTL6 0Ch
UCS control 7 UCSCTL7 0Eh
UCS control 8 UCSCTL8 10h

Table 6-23. SYS Registers (Base Address: 0180h)
REGISTER DESCRIPTION REGISTER OFFSET
System control SYSCTL 00h
Bootloader configuration area SYSBSLC 02h
JTAG mailbox control SYSJMBC 06h
JTAG mailbox input 0 SYSJMBI0 08h
JTAG mailbox input 1 SYSJMBI1 0Ah
JTAG mailbox output 0 SYSJMBO0 0Ch
JTAG mailbox output 1 SYSJMBO1 0Eh
Bus error vector generator SYSBERRIV 18h
User NMI vector generator SYSUNIV 1Ah
System NMI vector generator SYSSNIV 1Ch
Reset vector generator SYSRSTIV 1Eh

Table 6-24. Shared Reference Registers (Base Address: 01B0h)
REGISTER DESCRIPTION REGISTER OFFSET
Shared reference control REFCTL 00h

Table 6-25. Port Mapping Registers (Base Address: 01C0h)
REGISTER DESCRIPTION REGISTER OFFSET
Port mapping key and ID PMAPKEYID 00h
Port mapping control PMAPCTL 02h

Table 6-25. Port Mapping Registers (Base Address: 01E0h)
Port P4.0 mapping P4MAP0 00h
Port P4.1 mapping P4MAP1 01h
Port P4.2 mapping P4MAP2 02h
Port P4.3 mapping P4MAP3 03h
Port P4.4 mapping P4MAP4 04h
Port P4.5 mapping P4MAP5 05h
Port P4.6 mapping P4MAP6 06h
Port P4.7 mapping P4MAP7 07h

Table 6-26. Port P1 and P2 Registers (Base Address: 0200h)
REGISTER DESCRIPTION REGISTER OFFSET
Port P1 input P1IN 00h
Port P1 output P1OUT 02h
Port P1 direction P1DIR 04h
Port P1 resistor enable P1REN 06h
Port P1 drive strength P1DS 08h
Port P1 selection P1SEL 0Ah
Port P1 interrupt vector word P1IV 0Eh
Port P1 interrupt edge select P1IES 18h
Port P1 interrupt enable P1IE 1Ah
Port P1 interrupt flag P1IFG 1Ch
Port P2 input P2IN 01h
Port P2 output P2OUT 03h
Port P2 direction P2DIR 05h
Port P2 resistor enable P2REN 07h
Port P2 drive strength P2DS 09h
Port P2 selection P2SEL 0Bh
Port P2 interrupt vector word P2IV 1Eh
Port P2 interrupt edge select P2IES 19h
Port P2 interrupt enable P2IE 1Bh
Port P2 interrupt flag P2IFG 1Dh

Table 6-27. Port P3 and P4 Registers (Base Address: 0220h)
REGISTER DESCRIPTION REGISTER OFFSET
Port P3 input P3IN 00h
Port P3 output P3OUT 02h
Port P3 direction P3DIR 04h
Port P3 resistor enable P3REN 06h
Port P3 drive strength P3DS 08h
Port P3 selection P3SEL 0Ah
Port P4 input P4IN 01h
Port P4 output P4OUT 03h
Port P4 direction P4DIR 05h
Port P4 resistor enable P4REN 07h
Port P4 drive strength P4DS 09h
Port P4 selection P4SEL 0Bh

Table 6-28. Port P5 and P6 Registers (Base Address: 0240h)
REGISTER DESCRIPTION REGISTER OFFSET
Port P5 input P5IN 00h
Port P5 output P5OUT 02h
Port P5 direction P5DIR 04h
Port P5 resistor enable P5REN 06h
Port P5 drive strength P5DS 08h
Port P5 selection P5SEL 0Ah
Port P6 input P6IN 01h
Port P6 output P6OUT 03h
Port P6 direction P6DIR 05h
Port P6 resistor enable P6REN 07h
Port P6 drive strength P6DS 09h
Port P6 selection P6SEL 0Bh

Table 6-29. Port P7 and P8 Registers (Base Address: 0260h)
REGISTER DESCRIPTION REGISTER OFFSET
Port P7 input P7IN 00h
Port P7 output P7OUT 02h
Port P7 direction P7DIR 04h
Port P7 resistor enable P7REN 06h
Port P7 drive strength P7DS 08h
Port P7 selection P7SEL 0Ah
Port P8 input P8IN 01h
Port P8 output P8OUT 03h
Port P8 direction P8DIR 05h
Port P8 resistor enable P8REN 07h
Port P8 drive strength P8DS 09h
Port P8 selection P8SEL 0Bh

Table 6-30. Port J Registers (Base Address: 0320h)
REGISTER DESCRIPTION REGISTER OFFSET
Port PJ input PJIN 00h
Port PJ output PJOUT 02h
Port PJ direction PJDIR 04h
Port PJ resistor enable PJREN 06h
Port PJ drive strength PJDS 08h

Table 6-31. TA0 Registers (Base Address: 0340h)
REGISTER DESCRIPTION REGISTER OFFSET
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

Table 6-32. TA1 Registers (Base Address: 0380h)
REGISTER DESCRIPTION REGISTER OFFSET
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

Table 6-33. TB0 Registers (Base Address: 03C0h)
REGISTER DESCRIPTION REGISTER OFFSET
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

Table 6-34. TA2 Registers (Base Address: 0400h)
REGISTER DESCRIPTION REGISTER OFFSET
TA2 control TA2CTL 00h
Capture/compare control 0 TA2CCTL0 02h
Capture/compare control 1 TA2CCTL1 04h
Capture/compare control 2 TA2CCTL2 06h
TA2 counter TA2R 10h
Capture/compare 0 TA2CCR0 12h
Capture/compare 1 TA2CCR1 14h
Capture/compare 2 TA2CCR2 16h
TA2 expansion 0 TA2EX0 20h
TA2 interrupt vector TA2IV 2Eh

Table 6-35. Real-Time Clock Registers (Base Address: 04A0h)
REGISTER DESCRIPTION REGISTER OFFSET
RTC control 0 RTCCTL0 00h
RTC control 1 RTCCTL1 01h
RTC control 2 RTCCTL2 02h
RTC control 3 RTCCTL3 03h
RTC prescaler 0 control RTCPS0CTL 08h
RTC prescaler 1 control RTCPS1CTL 0Ah
RTC prescaler 0 RTCPS0 0Ch
RTC prescaler 1 RTCPS1 0Dh
RTC interrupt vector word RTCIV 0Eh

# The datasheet defined RTCSEC and RTCCNT1 together, RTCMIN and RTCCNT2 together, etc.
# I have broken these up into two separate sets.

# These are the SEC/MIN/HOUR/DOW versions.
RTC seconds RTCSEC 10h
RTC minutes RTCMIN 11h
RTC hours RTCHOUR 12h
RTC day of week RTCDOW 13h

# These are the RTCNTx versions.
RTC counter 1 RTCNT1 10h
RTC counter 2 RTCNT2 11h
RTC counter 3 RTCNT3 12h
RTC counter 4 RTCNT4 13h

RTC day RTCDAY 14h
RTC month RTCMON 15h
RTC year low RTCYEARL 16h
RTC year high RTCYEARH 17h
RTC alarm minutes RTCAMIN 18h
RTC alarm hours RTCAHOUR 19h
RTC alarm day of week RTCADOW 1Ah
RTC alarm days RTCADAY 1Bh

Table 6-36. 32-Bit Hardware Multiplier Registers (Base Address: 04C0h)
REGISTER DESCRIPTION REGISTER OFFSET
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

Table 6-37. DMA General Control (Base Address: 0500h)
DMA module control 0 DMACTL0 00h
DMA module control 1 DMACTL1 02h
DMA module control 2 DMACTL2 04h
DMA module control 3 DMACTL3 06h
DMA module control 4 DMACTL4 08h
DMA interrupt vector DMAIV 0Eh

Table 6-37. DMA Channel 0 (Base Address: 0510h)
DMA channel 0 control DMA0CTL 00h
DMA channel 0 source address low DMA0SAL 02h
DMA channel 0 source address high DMA0SAH 04h
DMA channel 0 destination address low DMA0DAL 06h
DMA channel 0 destination address high DMA0DAH 08h
DMA channel 0 transfer size DMA0SZ 0Ah

Table 6-37. DMA Channel 1 (Base Address: 0520h)
DMA channel 1 control DMA1CTL 00h
DMA channel 1 source address low DMA1SAL 02h
DMA channel 1 source address high DMA1SAH 04h
DMA channel 1 destination address low DMA1DAL 06h
DMA channel 1 destination address high DMA1DAH 08h
DMA channel 1 transfer size DMA1SZ 0Ah

Table 6-37. DMA Channel 2 (Base Address: 0530h)
DMA channel 2 control DMA2CTL 00h
DMA channel 2 source address low DMA2SAL 02h
DMA channel 2 source address high DMA2SAH 04h
DMA channel 2 destination address low DMA2DAL 06h
DMA channel 2 destination address high DMA2DAH 08h
DMA channel 2 transfer size DMA2SZ 0Ah

Table 6-38. USCI_A0 Registers (Base Address: 05C0h)
REGISTER DESCRIPTION REGISTER OFFSET
USCI control 1 UCA0CTL1 00h
USCI control 0 UCA0CTL0 01h
USCI baud rate 0 UCA0BR0 06h
USCI baud rate 1 UCA0BR1 07h
USCI modulation control UCA0MCTL 08h
USCI status UCA0STAT 0Ah
USCI receive buffer UCA0RXBUF 0Ch
USCI transmit buffer UCA0TXBUF 0Eh
USCI LIN control UCA0ABCTL 10h
USCI IrDA transmit control UCA0IRTCTL 12h
USCI IrDA receive control UCA0IRRCTL 13h
USCI interrupt enable UCA0IE 1Ch
USCI interrupt flags UCA0IFG 1Dh
USCI interrupt vector word UCA0IV 1Eh

Table 6-39. USCI_B0 Registers (Base Address: 05E0h)
REGISTER DESCRIPTION REGISTER OFFSET
USCI synchronous control 1 UCB0CTL1 00h
USCI synchronous control 0 UCB0CTL0 01h
USCI synchronous bit rate 0 UCB0BR0 06h
USCI synchronous bit rate 1 UCB0BR1 07h
USCI synchronous status UCB0STAT 0Ah
USCI synchronous receive buffer UCB0RXBUF 0Ch
USCI synchronous transmit buffer UCB0TXBUF 0Eh
USCI I2C own address UCB0I2COA 10h
USCI I2C slave address UCB0I2CSA 12h
USCI interrupt enable UCB0IE 1Ch
USCI interrupt flags UCB0IFG 1Dh
USCI interrupt vector word UCB0IV 1Eh

Table 6-40. USCI_A1 Registers (Base Address: 0600h)
REGISTER DESCRIPTION REGISTER OFFSET
USCI control 1 UCA1CTL1 00h
USCI control 0 UCA1CTL0 01h
USCI baud rate 0 UCA1BR0 06h
USCI baud rate 1 UCA1BR1 07h
USCI modulation control UCA1MCTL 08h
USCI status UCA1STAT 0Ah
USCI receive buffer UCA1RXBUF 0Ch
USCI transmit buffer UCA1TXBUF 0Eh
USCI LIN control UCA1ABCTL 10h
USCI IrDA transmit control UCA1IRTCTL 12h
USCI IrDA receive control UCA1IRRCTL 13h
USCI interrupt enable UCA1IE 1Ch
USCI interrupt flags UCA1IFG 1Dh
USCI interrupt vector word UCA1IV 1Eh

Table 6-41. USCI_B1 Registers (Base Address: 0620h)
REGISTER DESCRIPTION REGISTER OFFSET
USCI synchronous control 1 UCB1CTL1 00h
USCI synchronous control 0 UCB1CTL0 01h
USCI synchronous bit rate 0 UCB1BR0 06h
USCI synchronous bit rate 1 UCB1BR1 07h
USCI synchronous status UCB1STAT 0Ah
USCI synchronous receive buffer UCB1RXBUF 0Ch
USCI synchronous transmit buffer UCB1TXBUF 0Eh
USCI I2C own address UCB1I2COA 10h
USCI I2C slave address UCB1I2CSA 12h
USCI interrupt enable UCB1IE 1Ch
USCI interrupt flags UCB1IFG 1Dh
USCI interrupt vector word UCB1IV 1Eh

Table 6-42. ADC12_A Registers (Base Address: 0700h)
REGISTER DESCRIPTION REGISTER OFFSET
Control 0 ADC12CTL0 00h
Control 1 ADC12CTL1 02h
Control 2 ADC12CTL2 04h
Interrupt flag ADC12IFG 0Ah
Interrupt enable ADC12IE 0Ch
Interrupt vector word ADC12IV 0Eh
ADC memory control 0 ADC12MCTL0 10h
ADC memory control 1 ADC12MCTL1 11h
ADC memory control 2 ADC12MCTL2 12h
ADC memory control 3 ADC12MCTL3 13h
ADC memory control 4 ADC12MCTL4 14h
ADC memory control 5 ADC12MCTL5 15h
ADC memory control 6 ADC12MCTL6 16h
ADC memory control 7 ADC12MCTL7 17h
ADC memory control 8 ADC12MCTL8 18h
ADC memory control 9 ADC12MCTL9 19h
ADC memory control 10 ADC12MCTL10 1Ah
ADC memory control 11 ADC12MCTL11 1Bh
ADC memory control 12 ADC12MCTL12 1Ch
ADC memory control 13 ADC12MCTL13 1Dh
ADC memory control 14 ADC12MCTL14 1Eh
ADC memory control 15 ADC12MCTL15 1Fh
Conversion memory 0 ADC12MEM0 20h
Conversion memory 1 ADC12MEM1 22h
Conversion memory 2 ADC12MEM2 24h
Conversion memory 3 ADC12MEM3 26h
Conversion memory 4 ADC12MEM4 28h
Conversion memory 5 ADC12MEM5 2Ah
Conversion memory 6 ADC12MEM6 2Ch
Conversion memory 7 ADC12MEM7 2Eh
Conversion memory 8 ADC12MEM8 30h
Conversion memory 9 ADC12MEM9 32h
Conversion memory 10 ADC12MEM10 34h
Conversion memory 11 ADC12MEM11 36h
Conversion memory 12 ADC12MEM12 38h
Conversion memory 13 ADC12MEM13 3Ah
Conversion memory 14 ADC12MEM14 3Ch
Conversion memory 15 ADC12MEM15 3Eh

Table 6-43. Comparator_B Registers (Base Address: 08C0h)
REGISTER DESCRIPTION REGISTER OFFSET
Comp_B control 0 CBCTL0 00h
Comp_B control 1 CBCTL1 02h
Comp_B control 2 CBCTL2 04h
Comp_B control 3 CBCTL3 06h
Comp_B interrupt CBINT 0Ch
Comp_B interrupt vector word CBIV 0Eh

Table 6-44. USB Configuration Registers (Base Address: 0900h)
REGISTER DESCRIPTION REGISTER OFFSET
USB key and ID USBKEYID 00h
USB module configuration USBCNF 02h
USB PHY control USBPHYCTL 04h
USB power control USBPWRCTL 08h
USB PLL control USBPLLCTL 10h
USB PLL divider USBPLLDIV 12h
USB PLL interrupts USBPLLIR 14h

# NOTE: For some stupid reason, TI decided that, *in this one case*, to put
# underscores in the register names. Also, the names are stupid. I'm going to
# fix both of these problems. Of course, this is going to render my register
# incompatible with any example code...

Table 6-45. USB Control Registers (Base Address: 0920h)
REGISTER DESCRIPTION REGISTER OFFSET
Input endpoint_0 configuration USBIEP0CNF 00h
Input endpoint_0 byte count USBIEP0CNT 01h
Output endpoint_0 configuration USBOEP0CNF 02h
Output endpoint_0 byte count USBOEP0CNT 03h
Input endpoint interrupt enables USBIEPIE 0Eh
Output endpoint interrupt enables USBOEPIE 0Fh
Input endpoint interrupt flags USBIEPIFG 10h
Output endpoint interrupt flags USBOEPIFG 11h
USB interrupt vector USBIV 12h
USB maintenance USBMAINT 16h
Timestamp USBTSREG 18h
USB frame number USBFN 1Ah
USB control USBCTL 1Ch
USB interrupt enables USBIE 1Dh
USB interrupt flags USBIFG 1Eh
Function address USBFUNADR 1Fh
]]

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

# The datasheet defined RTCSEC and RTCCNT1 together, RTCMIN and RTCCNT2 together, etc.
# I have broken these up into two separate sets.

# These are the SEC/MIN/HOUR/DOW versions.
RTC seconds RTCSEC 10h
RTC minutes RTCMIN 11h
RTC hours RTCHOUR 12h
RTC day of week RTCDOW 13h

# These are the RTCNTx versions.
RTC counter 1 RTCNT1 10h
RTC counter 2 RTCNT2 11h
RTC counter 3 RTCNT3 12h
RTC counter 4 RTCNT4 13h

RTC day RTCDAY 14h
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
# Edited because it's a huge mess
# Removed "W0" from the end of the CRC32 input reg names
# Removed "W0" from the end of the CRC16 reg names
# CRC32RESRW0 was shown as CRC32RESRW1 - fixed this
CRC32 data input CRC32DI 00h
Reserved 02h
Reserved 04h
CRC32 data input reverse CRC32DIRB 06h
CRC32 initialization and result word 0 CRC32INIRESW0 08h
CRC32 initialization and result word 1 CRC32INIRESW1 0Ah
CRC32 result reverse word 1 CRC32RESRW1 0Ch
CRC32 result reverse word 0 CRC32RESRW0 0Eh
CRC16 data input CRC16DI 10h
Reserved 12h
Reserved 14h
CRC16 data input reverse CRC16DIRB 16h
CRC16 initialization and result word 0 CRC16INIRES 18h
Reserved 1Ah
Reserved 1Ch
CRC16 result reverse word 0 CRC16RESR 1Eh

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

dofile 'target/S08/device/string.lua'

function peripheral_printer()
    local drop = ""
    return function(peripheral, addr)
        print(fmt("%s( %s)\n%s", drop, peripheral, addr:lower()))
        drop = "drop\n\n"
    end
end

function print_reg(l)
    local comment, reg, offset = l:match "^(..-) (%w+) (%x+)h$"
    if comment then
        local padding = math.max(16 - reg:len(), 3)
        print(fmt("%s preg %s%s| %s",
            offset:lower(), reg, (" "):rep(padding), comment))
    end
end

-- This function is used to print *all* of the F5529 registers, which were
-- scraped verbatim from the datasheet and lightly edited in two cases to
-- break up a table into subtables.
function show_verbatim_regs(regs)
    local print_peripheral = peripheral_printer()
    for l in regs:lines() do
        if l ~= "" then
            -- skip comment lines and table heading lines
            if not l:match "^#" and not l:match "^REGISTER" then
                -- example: Table 6-18. Flash Control Registers (Base Address: 0140h)
                local peripheral, addr = l:match "^(Table ..-%. ..-) %(Base Address: (%x%x%x%x)h%)"
                if addr then
                    print_peripheral(peripheral, addr)
                else
                    print_reg(l)
                end
            end
        end
    end
    print "drop\n"
end

-- This function is used to print a subset of the FR6989 registers.
function show_regs(regs)
    local print_peripheral = peripheral_printer()
    for l in regs:lines() do
        if l ~= "" then
            if not l:match "^#" then    -- skip comment lines
                local addr, peripheral = l:match "^=(%x%x%x%x) (..-)$"
                if addr then
                    print_peripheral(peripheral, addr)
                else
                    print_reg(l)
                end
            end
        end
    end
    print "drop\n"
end

function show_part(name, vecs, regs, show_fn)
    print (string.format(
        "( Equates for MSP430%s. Extracted from the datasheet using Lua!)\n", name))
    --show_vectors(vecs)
    show_fn(regs)
    print (string.format(
        "( End of auto-generated equates for MSP430%s.)\n", name))
end

show_part("F5529", nil, f5529_regs, show_verbatim_regs)
show_part("FR6989", nil, fr6989_regs, show_regs)
