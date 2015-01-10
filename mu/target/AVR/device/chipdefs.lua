-- This file is part of muFORTH: http://muforth.nimblemachines.com/

-- Copyright 2002-2015 David Frech. All rights reserved, and all wrongs
-- reversed. (See the file COPYRIGHT for details.)

-- Common equates for Atmel atmega parts.

-- Be careful of pasted in endashes and ligatures! fl really got me...
-- To convert en- and em-dashes to normal (double) dashes, do this (in Vim):
-- :set encoding=utf-8
-- :%s/\%u2013/--/g  ( en-dash)
-- :%s/\%u2014/--/g  ( em-dash)
-- and for common ligatures, do these:
-- :%s/\%ufb00/ff/g
-- :%s/\%ufb01/fi/g
-- :%s/\%ufb02/fl/g
-- :%s/\%ufb03/ffi/g
-- :%s/\%ufb04/ffl/g

megax4_reg = [[
(0xCE) UDR1 USART1 I/O Data Register 189
(0xCD) UBRR1H -- -- -- -- USART1 Baud Rate Register High Byte 193/206
(0xCC) UBRR1L USART1 Baud Rate Register Low Byte 193/206
(0xCB) Reserved -- -- -- -- -- -- -- --
(0xCA) UCSR1C UMSEL11 UMSEL10 -- -- -- UDORD1 UCPHA1 UCPOL1 191/205
(0xC9) UCSR1B RXCIE1 TXCIE1 UDRIE1 RXEN1 TXEN1 UCSZ12 RXB81 TXB81 190/204
(0xC8) UCSR1A RXC1 TXC1 UDRE1 FE1 DOR1 UPE1 U2X1 MPCM1 189/204
(0xC7) Reserved -- -- -- -- -- -- -- --
(0xC6) UDR0 USART0 I/O Data Register 189
(0xC5) UBRR0H -- -- -- -- USART0 Baud Rate Register High Byte 193/206
(0xC4) UBRR0L USART0 Baud Rate Register Low Byte 193/206
(0xC3) Reserved -- -- -- -- -- -- -- --
(0xC2) UCSR0C UMSEL01 UMSEL00 -- -- -- UDORD0 UCPHA0 UCPOL0 191/205
(0xC1) UCSR0B RXCIE0 TXCIE0 UDRIE0 RXEN0 TXEN0 UCSZ02 RXB80 TXB80 190/204
(0xC0) UCSR0A RXC0 TXC0 UDRE0 FE0 DOR0 UPE0 U2X0 MPCM0 189/204
(0xBF) Reserved -- -- -- -- -- -- -- --
(0xBE) Reserved -- -- -- -- -- -- -- --
(0xBD) TWAMR TWAM6 TWAM5 TWAM4 TWAM3 TWAM2 TWAM1 TWAM0 -- 235
(0xBC) TWCR TWINT TWEA TWSTA TWSTO TWWC TWEN -- TWIE 232
(0xBB) TWDR 2-wire Serial Interface Data Register 234
(0xBA) TWAR TWA6 TWA5 TWA4 TWA3 TWA2 TWA1 TWA0 TWGCE 235
(0xB9) TWSR TWS7 TWS6 TWS5 TWS4 TWS3 -- TWPS1 TWPS0 234
(0xB8) TWBR 2-wire Serial Interface Bit Rate Register 232
(0xB7) Reserved -- -- -- -- -- -- -- --
(0xB6) ASSR -- EXCLK AS2 TCN2UB OCR2AUB OCR2BUB TCR2AUB TCR2BUB 157
(0xB5) Reserved -- -- -- -- -- -- -- --
(0xB4) OCR2B Timer/Counter2 Output Compare Register B 157
(0xB3) OCR2A Timer/Counter2 Output Compare Register A 157
(0xB2) TCNT2 Timer/Counter2 (8 Bit) 156
(0xB1) TCCR2B FOC2A FOC2B -- -- WGM22 CS22 CS21 CS20 155
(0xB0) TCCR2A COM2A1 COM2A0 COM2B1 COM2B0 -- -- WGM21 WGM20 152
(0x8B) OCR1BH Timer/Counter1 - Output Compare Register B High Byte 136
(0x8A) OCR1BL Timer/Counter1 - Output Compare Register B Low Byte 136
(0x89) OCR1AH Timer/Counter1 - Output Compare Register A High Byte 136
(0x88) OCR1AL Timer/Counter1 - Output Compare Register A Low Byte 136
(0x87) ICR1H Timer/Counter1 - Input Capture Register High Byte 137
(0x86) ICR1L Timer/Counter1 - Input Capture Register Low Byte 137
(0x85) TCNT1H Timer/Counter1 - Counter Register High Byte 136
(0x84) TCNT1L Timer/Counter1 - Counter Register Low Byte 136
(0x83) Reserved -- -- -- -- -- -- -- --
(0x82) TCCR1C FOC1A FOC1B -- -- -- -- -- -- 135
(0x81) TCCR1B ICNC1 ICES1 -- WGM13 WGM12 CS12 CS11 CS10 134
(0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 -- -- WGM11 WGM10 132
(0x7F) DIDR1 -- -- -- -- -- -- AIN1D AIN0D 239
(0x7E) DIDR0 ADC7D ADC6D ADC5D ADC4D ADC3D ADC2D ADC1D ADC0D 259
(0x7D) Reserved -- -- -- -- -- -- -- --
(0x7C) ADMUX REFS1 REFS0 ADLAR MUX4 MUX3 MUX2 MUX1 MUX0 255
(0x7B) ADCSRB -- ACME -- -- -- ADTS2 ADTS1 ADTS0 238
(0x7A) ADCSRA ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0 257
(0x79) ADCH ADC Data Register High byte 258
(0x78) ADCL ADC Data Register Low byte 258
(0x77) Reserved -- -- -- -- -- -- -- --
(0x76) Reserved -- -- -- -- -- -- -- --
(0x75) Reserved -- -- -- -- -- -- -- --
(0x74) Reserved -- -- -- -- -- -- -- --
(0x73) PCMSK3 PCINT31 PCINT30 PCINT29 PCINT28 PCINT27 PCINT26 PCINT25 PCINT24 70
(0x72) Reserved -- -- -- -- -- -- -- --
(0x71) Reserved -- -- -- -- -- -- -- --
(0x70) TIMSK2 -- -- -- -- -- OCIE2B OCIE2A TOIE2 158
(0x6F) TIMSK1 -- -- ICIE1 -- -- OCIE1B OCIE1A TOIE1 137
(0x6E) TIMSK0 -- -- -- -- -- OCIE0B OCIE0A TOIE0 109
(0x6D) PCMSK2 PCINT23 PCINT22 PCINT21 PCINT20 PCINT19 PCINT18 PCINT17 PCINT16 70
(0x6C) PCMSK1 PCINT15 PCINT14 PCINT13 PCINT12 PCINT11 PCINT10 PCINT9 PCINT8 70
(0x6B) PCMSK0 PCINT7 PCINT6 PCINT5 PCINT4 PCINT3 PCINT2 PCINT1 PCINT0 71
(0x6A) Reserved -- -- -- -- -- -- -- --
(0x69) EICRA -- -- ISC21 ISC20 ISC11 ISC10 ISC01 ISC00 67
(0x68) PCICR -- -- -- -- PCIE3 PCIE2 PCIE1 PCIE0 69
(0x67) Reserved -- -- -- -- -- -- -- --
(0x66) OSCCAL Oscillator Calibration Register 40
(0x65) Reserved -- -- -- -- -- -- -- --
(0x64) PRR PRTWI PRTIM2 PRTIM0 PRUSART1 PRTIM1 PRSPI PRUSART0 PRADC 48
(0x63) Reserved -- -- -- -- -- -- -- --
(0x62) Reserved -- -- -- -- -- -- -- --
(0x61) CLKPR CLKPCE -- -- -- CLKPS3 CLKPS2 CLKPS1 CLKPS0 40
(0x60) WDTCSR WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0 59
0x3F (0x5F) SREG I T H S V N Z C 10
0x3E (0x5E) SPH SP15 SP14 SP13 SP12 SP11 SP10 SP9 SP8 11
0x3D (0x5D) SPL SP7 SP6 SP5 SP4 SP3 SP2 SP1 SP0 11
0x3C (0x5C) Reserved -- -- -- -- -- -- -- --
0x3B (0x5B) RAMPZ -- -- -- -- -- -- -- RAMPZ0 14
0x3A (0x5A) Reserved -- -- -- -- -- -- -- --
0x39 (0x59) Reserved -- -- -- -- -- -- -- --
0x38 (0x58) Reserved -- -- -- -- -- -- -- --
0x37 (0x57) SPMCSR SPMIE RWWSB SIGRD RWWSRE BLBSET PGWRT PGERS SPMEN 291
0x36 (0x56) Reserved -- -- -- -- -- -- -- --
0x35 (0x55) MCUCR JTD BODS BODSE PUD -- -- IVSEL IVCE 91/275
0x34 (0x54) MCUSR -- -- -- JTRF WDRF BORF EXTRF PORF 58/275
0x33 (0x53) SMCR -- -- -- -- SM2 SM1 SM0 SE 47
0x32 (0x52) Reserved -- -- -- -- -- -- -- --
0x31 (0x51) OCDR On-Chip Debug Register 265
0x30 (0x50) ACSR ACD ACBG ACO ACI ACIE ACIC ACIS1 ACIS0 257
0x2F (0x4F) Reserved -- -- -- -- -- -- -- --
0x2E (0x4E) SPDR SPI 0 Data Register 170
0x2D (0x4D) SPSR SPIF0 WCOL0 -- -- -- -- -- SPI2X0 169
0x2C (0x4C) SPCR SPIE0 SPE0 DORD0 MSTR0 CPOL0 CPHA0 SPR01 SPR00 168
0x2B (0x4B) GPIOR2 General Purpose I/O Register 2 28
0x2A (0x4A) GPIOR1 General Purpose I/O Register 1 28
0x29 (0x49) Reserved -- -- -- -- -- -- -- --
0x28 (0x48) OCR0B Timer/Counter0 Output Compare Register B 109
0x27 (0x47) OCR0A Timer/Counter0 Output Compare Register A 108
0x26 (0x46) TCNT0 Timer/Counter0 (8 Bit) 108
0x25 (0x45) TCCR0B FOC0A FOC0B -- -- WGM02 CS02 CS01 CS00 107
0x24 (0x44) TCCR0A COM0A1 COM0A0 COM0B1 COM0B0 -- -- WGM01 WGM00 109
0x23 (0x43) GTCCR TSM -- -- -- -- -- PSRASY PSR5SYNC 159
0x22 (0x42) EEARH -- -- -- -- EEPROM Address Register High Byte 23
0x21 (0x41) EEARL EEPROM Address Register Low Byte 23
0x20 (0x40) EEDR EEPROM Data Register 23
0x1F (0x3F) EECR -- -- EEPM1 EEPM0 EERIE EEMPE EEPE EERE 23
0x1E (0x3E) GPIOR0 General Purpose I/O Register 0 28
0x1D (0x3D) EIMSK -- -- -- -- -- INT2 INT1 INT0 68
0x1C (0x3C) EIFR -- -- -- -- -- INTF2 INTF1 INTF0 68
0x1B (0x3B) PCIFR -- -- -- -- PCIF3 PCIF2 PCIF1 PCIF0 69
0x1A (0x3A) Reserved -- -- -- -- -- -- -- --
0x19 (0x39) Reserved -- -- -- -- -- -- -- --
0x18 (0x38) Reserved -- -- -- -- -- -- -- --
0x17 (0x37) TIFR2 -- -- -- -- -- OCF2B OCF2A TOV2 159
0x16 (0x36) TIFR1 -- -- ICF1 -- -- OCF1B OCF1A TOV1 138
0x15 (0x35) TIFR0 -- -- -- -- -- OCF0B OCF0A TOV0 109
0x14 (0x34) Reserved -- -- -- -- -- -- -- --
0x13 (0x33) Reserved -- -- -- -- -- -- -- --
0x12 (0x32) Reserved -- -- -- -- -- -- -- --
0x11 (0x31) Reserved -- -- -- -- -- -- -- --
0x10 (0x30) Reserved -- -- -- -- -- -- -- --
0x0F (0x2F) Reserved -- -- -- -- -- -- -- --
0x0E (0x2E) Reserved -- -- -- -- -- -- -- --
0x0D (0x2D) Reserved -- -- -- -- -- -- -- --
0x0C (0x2C) Reserved -- -- -- -- -- -- -- --
0x0B (0x2B) PORTD PORTD7 PORTD6 PORTD5 PORTD4 PORTD3 PORTD2 PORTD1 PORTD0 92
0x0A (0x2A) DDRD DDD7 DDD6 DDD5 DDD4 DDD3 DDD2 DDD1 DDD0 92
0x09 (0x29) PIND PIND7 PIND6 PIND5 PIND4 PIND3 PIND2 PIND1 PIND0 92
0x08 (0x28) PORTC PORTC7 PORTC6 PORTC5 PORTC4 PORTC3 PORTC2 PORTC1 PORTC0 92
0x07 (0x27) DDRC DDC7 DDC6 DDC5 DDC4 DDC3 DDC2 DDC1 DDC0 92
0x06 (0x26) PINC PINC7 PINC6 PINC5 PINC4 PINC3 PINC2 PINC1 PINC0 92
0x05 (0x25) PORTB PORTB7 PORTB6 PORTB5 PORTB4 PORTB3 PORTB2 PORTB1 PORTB0 91
0x04 (0x24) DDRB DDB7 DDB6 DDB5 DDB4 DDB3 DDB2 DDB1 DDB0 91
0x03 (0x23) PINB PINB7 PINB6 PINB5 PINB4 PINB3 PINB2 PINB1 PINB0 91
0x02 (0x22) PORTA PORTA7 PORTA6 PORTA5 PORTA4 PORTA3 PORTA2 PORTA1 PORTA0 91
0x01 (0x21) DDRA DDA7 DDA6 DDA5 DDA4 DDA3 DDA2 DDA1 DDA0 91
0x00 (0x20) PINA PINA7 PINA6 PINA5 PINA4 PINA3 PINA2 PINA1 PINA0 91
]]

megax4_vec = [[
1 $0000 RESET External Pin, Power-on Reset, Brown-out Reset, Watchdog Reset, and JTAG AVR Reset
2 $0002 INT0 External Interrupt Request 0
3 $0004 INT1 External Interrupt Request 1
4 $0006 INT2 External Interrupt Request 2
5 $0008 PCINT0 Pin Change Interrupt Request 0
6 $000A PCINT1 Pin Change Interrupt Request 1
7 $000C PCINT2 Pin Change Interrupt Request 2
8 $000E PCINT3 Pin Change Interrupt Request 3
9 $0010 WDT Watchdog Time-out Interrupt
10 $0012 TIMER2_COMPA Timer/Counter2 Compare Match A
11 $0014 TIMER2_COMPB Timer/Counter2 Compare Match B
12 $0016 TIMER2_OVF Timer/Counter2 Overflow
13 $0018 TIMER1_CAPT Timer/Counter1 Capture Event
14 $001A TIMER1_COMPA Timer/Counter1 Compare Match A
15 $001C TIMER1_COMPB Timer/Counter1 Compare Match B
16 $001E TIMER1_OVF Timer/Counter1 Overflow
17 $0020 TIMER0_COMPA Timer/Counter0 Compare Match A
18 $0022 TIMER0_COMPB Timer/Counter0 Compare match B
19 $0024 TIMER0_OVF Timer/Counter0 Overflow
20 $0026 SPI_STC SPI Serial Transfer Complete
21 $0028 USART0_RX USART0 Rx Complete
22 $002A USART0_UDRE USART0 Data Register Empty
23 $002C USART0_TX USART0 Tx Complete
24 $002E ANALOG_COMP Analog Comparator
25 $0030 ADC ADC Conversion Complete
26 $0032 EE_READY EEPROM Ready
27 $0034 TWI 2-wire Serial Interface
28 $0036 SPM_READY Store Program Memory Ready
29 $0038 USART1_RX USART1 Rx Complete
30 $003A USART1_UDRE USART1 Data Register Empty
31 $003C USART1_TX USART1 Tx Complete
]]

megax8_reg = [[
(0xC6) UDR0 USART I/O Data Register 193
(0xC5) UBRR0H USART Baud Rate Register High 197
(0xC4) UBRR0L USART Baud Rate Register Low 197
(0xC3) Reserved -- -- -- -- -- -- -- --
(0xC2) UCSR0C UMSEL01 UMSEL00 UPM01 UPM00 USBS0 UCSZ01/UDORD0 UCSZ00/UCPHA0 UCPOL0 195/210
(0xC1) UCSR0B RXCIE0 TXCIE0 UDRIE0 RXEN0 TXEN0 UCSZ02 RXB80 TXB80 194
(0xC0) UCSR0A RXC0 TXC0 UDRE0 FE0 DOR0 UPE0 U2X0 MPCM0 193
(0xBF) Reserved -- -- -- -- -- -- -- --
(0xBE) Reserved -- -- -- -- -- -- -- --
(0xBD) TWAMR TWAM6 TWAM5 TWAM4 TWAM3 TWAM2 TWAM1 TWAM0 -- 242
(0xBC) TWCR TWINT TWEA TWSTA TWSTO TWWC TWEN -- TWIE 239
(0xBB) TWDR 2-wire Serial Interface Data Register 241
(0xBA) TWAR TWA6 TWA5 TWA4 TWA3 TWA2 TWA1 TWA0 TWGCE 242
(0xB9) TWSR TWS7 TWS6 TWS5 TWS4 TWS3 -- TWPS1 TWPS0 241
(0xB8) TWBR 2-wire Serial Interface Bit Rate Register 239
(0xB7) Reserved -- -- -- -- -- -- --
(0xB6) ASSR -- EXCLK AS2 TCN2UB OCR2AUB OCR2BUB TCR2AUB TCR2BUB 162
(0xB5) Reserved -- -- -- -- -- -- -- --
(0xB4) OCR2B Timer/Counter2 Output Compare Register B 160
(0xB3) OCR2A Timer/Counter2 Output Compare Register A 160
(0xB2) TCNT2 Timer/Counter2 (8-bit) 160
(0xB1) TCCR2B FOC2A FOC2B -- -- WGM22 CS22 CS21 CS20 159
(0xB0) TCCR2A COM2A1 COM2A0 COM2B1 COM2B0 -- -- WGM21 WGM20 156
(0x8B) OCR1BH Timer/Counter1 - Output Compare Register B High Byte 136
(0x8A) OCR1BL Timer/Counter1 - Output Compare Register B Low Byte 136
(0x89) OCR1AH Timer/Counter1 - Output Compare Register A High Byte 136
(0x88) OCR1AL Timer/Counter1 - Output Compare Register A Low Byte 136
(0x87) ICR1H Timer/Counter1 - Input Capture Register High Byte 137
(0x86) ICR1L Timer/Counter1 - Input Capture Register Low Byte 137
(0x85) TCNT1H Timer/Counter1 - Counter Register High Byte 136
(0x84) TCNT1L Timer/Counter1 - Counter Register Low Byte 136
(0x83) Reserved -- -- -- -- -- -- -- --
(0x82) TCCR1C FOC1A FOC1B -- -- -- -- -- -- 135
(0x81) TCCR1B ICNC1 ICES1 -- WGM13 WGM12 CS12 CS11 CS10 134
(0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 -- -- WGM11 WGM10 132
(0x7F) DIDR1 -- -- -- -- -- -- AIN1D AIN0D 247
(0x7E) DIDR0 -- -- ADC5D ADC4D ADC3D ADC2D ADC1D ADC0D 264
(0x7D) Reserved -- -- -- -- -- -- -- --
(0x7C) ADMUX REFS1 REFS0 ADLAR -- MUX3 MUX2 MUX1 MUX0 260
(0x7B) ADCSRB -- ACME -- -- -- ADTS2 ADTS1 ADTS0 263
(0x7A) ADCSRA ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0 261
(0x79) ADCH ADC Data Register High byte 263
(0x78) ADCL ADC Data Register Low byte 263
(0x77) Reserved -- -- -- -- -- -- -- --
(0x76) Reserved -- -- -- -- -- -- -- --
(0x75) Reserved -- -- -- -- -- -- -- --
(0x74) Reserved -- -- -- -- -- -- -- --
(0x73) Reserved -- -- -- -- -- -- -- --
(0x72) Reserved -- -- -- -- -- -- -- --
(0x71) Reserved -- -- -- -- -- -- -- --
(0x70) TIMSK2 -- -- -- -- -- OCIE2B OCIE2A TOIE2 161
(0x6F) TIMSK1 -- -- ICIE1 -- -- OCIE1B OCIE1A TOIE1 137
(0x6E) TIMSK0 -- -- -- -- -- OCIE0B OCIE0A TOIE0 109
(0x6D) PCMSK2 PCINT23 PCINT22 PCINT21 PCINT20 PCINT19 PCINT18 PCINT17 PCINT16 72
(0x6C) PCMSK1 -- PCINT14 PCINT13 PCINT12 PCINT11 PCINT10 PCINT9 PCINT8 72
(0x6B) PCMSK0 PCINT7 PCINT6 PCINT5 PCINT4 PCINT3 PCINT2 PCINT1 PCINT0 72
(0x6A) Reserved -- -- -- -- -- -- -- --
(0x69) EICRA -- -- -- -- ISC11 ISC10 ISC01 ISC00 69
(0x68) PCICR -- -- -- -- -- PCIE2 PCIE1 PCIE0
(0x67) Reserved -- -- -- -- -- -- -- --
(0x66) OSCCAL Oscillator Calibration Register 38
(0x65) Reserved -- -- -- -- -- -- -- --
(0x64) PRR PRTWI PRTIM2 PRTIM0 -- PRTIM1 PRSPI PRUSART0 PRADC 43
(0x63) Reserved -- -- -- -- -- -- -- --
(0x62) Reserved -- -- -- -- -- -- -- --
(0x61) CLKPR CLKPCE -- -- -- CLKPS3 CLKPS2 CLKPS1 CLKPS0 38
(0x60) WDTCSR WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0 55
0x3F (0x5F) SREG I T H S V N Z C 10
0x3E (0x5E) SPH -- -- -- -- -- SP10 SP9 SP8 13
0x3D (0x5D) SPL SP7 SP6 SP5 SP4 SP3 SP2 SP1 SP0 13
0x3C (0x5C) Reserved -- -- -- -- -- -- -- --
0x3B (0x5B) Reserved -- -- -- -- -- -- -- --
0x3A (0x5A) Reserved -- -- -- -- -- -- -- --
0x39 (0x59) Reserved -- -- -- -- -- -- -- --
0x38 (0x58) Reserved -- -- -- -- -- -- -- --
0x37 (0x57) SPMCSR SPMIE RWWSB -- RWWSRE BLBSET PGWRT PGERS SELFPRGEN 289
0x36 (0x56) Reserved -- -- -- -- -- -- -- --
0x35 (0x55) MCUCR -- BODS BODSE PUD -- -- IVSEL IVCE 45/66/90
0x34 (0x54) MCUSR -- -- -- -- WDRF BORF EXTRF PORF 55
0x33 (0x53) SMCR -- -- -- -- SM2 SM1 SM0 SE 41
0x32 (0x52) Reserved -- -- -- -- -- -- -- --
0x31 (0x51) Reserved -- -- -- -- -- -- -- --
0x30 (0x50) ACSR ACD ACBG ACO ACI ACIE ACIC ACIS1 ACIS0 245
0x2F (0x4F) Reserved -- -- -- -- -- -- -- --
0x2E (0x4E) SPDR SPI Data Register 173
0x2D (0x4D) SPSR SPIF WCOL -- -- -- -- -- SPI2X 172
0x2C (0x4C) SPCR SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0 171
0x2B (0x4B) GPIOR2 General Purpose I/O Register 2 26
0x2A (0x4A) GPIOR1 General Purpose I/O Register 1 26
0x29 (0x49) Reserved -- -- -- -- -- -- -- --
0x28 (0x48) OCR0B Timer/Counter0 Output Compare Register B
0x27 (0x47) OCR0A Timer/Counter0 Output Compare Register A
0x26 (0x46) TCNT0 Timer/Counter0 (8-bit)
0x25 (0x45) TCCR0B FOC0A FOC0B -- -- WGM02 CS02 CS01 CS00
0x24 (0x44) TCCR0A COM0A1 COM0A0 COM0B1 COM0B0 -- -- WGM01 WGM00
0x23 (0x43) GTCCR TSM -- -- -- -- -- PSRASY PSRSYNC 141/163
0x22 (0x42) EEARH (EEPROM Address Register High Byte) 5. 22
0x21 (0x41) EEARL EEPROM Address Register Low Byte 22
0x20 (0x40) EEDR EEPROM Data Register 22
0x1F (0x3F) EECR -- -- EEPM1 EEPM0 EERIE EEMPE EEPE EERE 22
0x1E (0x3E) GPIOR0 General Purpose I/O Register 0 26
0x1D (0x3D) EIMSK -- -- -- -- -- -- INT1 INT0 70
0x1C (0x3C) EIFR -- -- -- -- -- -- INTF1 INTF0 70
0x1B (0x3B) PCIFR -- -- -- -- -- PCIF2 PCIF1 PCIF0
0x1A (0x3A) Reserved -- -- -- -- -- -- -- --
0x19 (0x39) Reserved -- -- -- -- -- -- -- --
0x18 (0x38) Reserved -- -- -- -- -- -- -- --
0x17 (0x37) TIFR2 -- -- -- -- -- OCF2B OCF2A TOV2 161
0x16 (0x36) TIFR1 -- -- ICF1 -- -- OCF1B OCF1A TOV1 138
0x15 (0x35) TIFR0 -- -- -- -- -- OCF0B OCF0A TOV0
0x14 (0x34) Reserved -- -- -- -- -- -- -- --
0x13 (0x33) Reserved -- -- -- -- -- -- -- --
0x12 (0x32) Reserved -- -- -- -- -- -- -- --
0x11 (0x31) Reserved -- -- -- -- -- -- -- --
0x10 (0x30) Reserved -- -- -- -- -- -- -- --
0x0F (0x2F) Reserved -- -- -- -- -- -- -- --
0x0E (0x2E) Reserved -- -- -- -- -- -- -- --
0x0D (0x2D) Reserved -- -- -- -- -- -- -- --
0x0C (0x2C) Reserved -- -- -- -- -- -- -- --
0x0B (0x2B) PORTD PORTD7 PORTD6 PORTD5 PORTD4 PORTD3 PORTD2 PORTD1 PORTD0 91
0x0A (0x2A) DDRD DDD7 DDD6 DDD5 DDD4 DDD3 DDD2 DDD1 DDD0 91
0x09 (0x29) PIND PIND7 PIND6 PIND5 PIND4 PIND3 PIND2 PIND1 PIND0 91
0x08 (0x28) PORTC -- PORTC6 PORTC5 PORTC4 PORTC3 PORTC2 PORTC1 PORTC0 90
0x07 (0x27) DDRC -- DDC6 DDC5 DDC4 DDC3 DDC2 DDC1 DDC0 90
0x06 (0x26) PINC -- PINC6 PINC5 PINC4 PINC3 PINC2 PINC1 PINC0 90
0x05 (0x25) PORTB PORTB7 PORTB6 PORTB5 PORTB4 PORTB3 PORTB2 PORTB1 PORTB0 90
0x04 (0x24) DDRB DDB7 DDB6 DDB5 DDB4 DDB3 DDB2 DDB1 DDB0 90
0x03 (0x23) PINB PINB7 PINB6 PINB5 PINB4 PINB3 PINB2 PINB1 PINB0 90
]]

megax8_vec = [[
1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog System Reset
2 0x001 INT0 External Interrupt Request 0
3 0x002 INT1 External Interrupt Request 1
4 0x003 PCINT0 Pin Change Interrupt Request 0
5 0x004 PCINT1 Pin Change Interrupt Request 1
6 0x005 PCINT2 Pin Change Interrupt Request 2
7 0x006 WDT Watchdog Time-out Interrupt
8 0x007 TIMER2_COMPA Timer/Counter2 Compare Match A
9 0x008 TIMER2_COMPB Timer/Counter2 Compare Match B
10 0x009 TIMER2_OVF Timer/Counter2 Overflow
11 0x00A TIMER1_CAPT Timer/Counter1 Capture Event
12 0x00B TIMER1_COMPA Timer/Counter1 Compare Match A
13 0x00C TIMER1_COMPB Timer/Coutner1 Compare Match B
14 0x00D TIMER1_OVF Timer/Counter1 Overflow
15 0x00E TIMER0_COMPA Timer/Counter0 Compare Match A
16 0x00F TIMER0_COMPB Timer/Counter0 Compare Match B
17 0x010 TIMER0_OVF Timer/Counter0 Overflow
18 0x011 SPI_STC SPI Serial Transfer Complete
19 0x012 USART_RX USART Rx Complete
20 0x013 USART_UDRE USART Data Register Empty
21 0x014 USART_TX USART Tx Complete
22 0x015 ADC ADC Conversion Complete
23 0x016 EE_READY EEPROM Ready
24 0x017 ANALOG_COMP Analog Comparator
25 0x018 TWI 2-wire Serial Interface
26 0x019 SPM_READY Store Program Memory Ready
]]

mega8515_reg = [[
$3F ($5F) SREG I T H S V N Z C 10
$3E ($5E) SPH SP15 SP14 SP13 SP12 SP11 SP10 SP9 SP8 12
$3D ($5D) SPL SP7 SP6 SP5 SP4 SP3 SP2 SP1 SP0 12
$3C ($5C) Reserved -
$3B ($5B) GICR INT1 INT0 INT2 -- -- -- IVSEL IVCE 57, 78
$3A ($5A) GIFR INTF1 INTF0 INTF2 -- -- -- -- -- 79
$39 ($59) TIMSK TOIE1 OCIE1A OCIE1B -- TICIE1 -- TOIE0 OCIE0 93, 124
$38 ($58) TIFR TOV1 OCF1A OCF1B -- ICF1 -- TOV0 OCF0 94, 125
$37 ($57) SPMCR SPMIE RWWSB -- RWWSRE BLBSET PGWRT PGERS SPMEN 170
$36 ($56) EMCUCR SM0 SRL2 SRL1 SRL0 SRW01 SRW00 SRW11 ISC2 29,42,78
$35 ($55) MCUCR SRE SRW10 SE SM1 ISC11 ISC10 ISC01 ISC00 29,41,77
$34 ($54) MCUCSR -- -- SM2 -- WDRF BORF EXTRF PORF 41,49
$33 ($53) TCCR0 FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00 91
$32 ($52) TCNT0 Timer/Counter0 (8 Bits) 93
$31 ($51) OCR0 Timer/Counter0 Output Compare Register 93
$30 ($50) SFIOR -- XMBK XMM2 XMM1 XMM0 PUD -- PSR10 31,66,96
$2F ($4F) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10 119
$2E ($4E) TCCR1B ICNC1 ICES1 -- WGM13 WGM12 CS12 CS11 CS10 122
$2D ($4D) TCNT1H Timer/Counter1 - Counter Register High Byte 123
$2C ($4C) TCNT1L Timer/Counter1 - Counter Register Low Byte 123
$2B ($4B) OCR1AH Timer/Counter1 - Output Compare Register A High Byte 123
$2A ($4A) OCR1AL Timer/Counter1 - Output Compare Register A Low Byte 123
$29 ($49) OCR1BH Timer/Counter1 - Output Compare Register B High Byte 123
$28 ($48) OCR1BL Timer/Counter1 - Output Compare Register B Low Byte 123
$27 ($47) Reserved - -
$26 ($46) Reserved - -
$25 ($45) ICR1H Timer/Counter1 - Input Capture Register High Byte 124
$24 ($44) ICR1L Timer/Counter1 - Input Capture Register Low Byte 124
$23 ($43) Reserved - -
$22 ($42) Reserved - -
$21 ($41) WDTCR -- -- -- WDCE WDE WDP2 WDP1 WDP0 51
$20 ($40) UBRRH URSEL -- -- -- UBRR[11:8] 159
$20 ($40) UUCSRC URSEL UMSEL UPM1 UPM0 USBS UCSZ1 UCSZ0 UCPOL 157
$1F ($3F) EEARH -- -- -- -- -- -- -- EEAR8 19
$1E ($3E) EEARL EEPROM Address Register Low Byte 19
$1D ($3D) EEDR EEPROM Data Register 20
$1C ($3C) EECR -- -- -- -- EERIE EEMWE EEWE EERE 20
$1B ($3B) PORTA PORTA7 PORTA6 PORTA5 PORTA4 PORTA3 PORTA2 PORTA1 PORTA0 75
$1A ($3A) DDRA DDA7 DDA6 DDA5 DDA4 DDA3 DDA2 DDA1 DDA0 75
$19 ($39) PINA PINA7 PINA6 PINA5 PINA4 PINA3 PINA2 PINA1 PINA0 75
$18 ($38) PORTB PORTB7 PORTB6 PORTB5 PORTB4 PORTB3 PORTB2 PORTB1 PORTB0 75
$17 ($37) DDRB DDB7 DDB6 DDB5 DDB4 DDB3 DDB2 DDB1 DDB0 75
$16 ($36) PINB PINB7 PINB6 PINB5 PINB4 PINB3 PINB2 PINB1 PINB0 75
$15 ($35) PORTC PORTC7 PORTC6 PORTC5 PORTC4 PORTC3 PORTC2 PORTC1 PORTC0 75
$14 ($34) DDRC DDC7 DDC6 DDC5 DDC4 DDC3 DDC2 DDC1 DDC0 75
$13 ($33) PINC PINC7 PINC6 PINC5 PINC4 PINC3 PINC2 PINC1 PINC0 76
$12 ($32) PORTD PORTD7 PORTD6 PORTD5 PORTD4 PORTD3 PORTD2 PORTD1 PORTD0 76
$11 ($31) DDRD DDD7 DDD6 DDD5 DDD4 DDD3 DDD2 DDD1 DDD0 76
$10 ($30) PIND PIND7 PIND6 PIND5 PIND4 PIND3 PIND2 PIND1 PIND0 76
$0F ($2F) SPDR SPI Data Register 133
$0E ($2E) SPSR SPIF WCOL -- -- -- -- -- SPI2X 133
$0D ($2D) SPCR SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0 131
$0C ($2C) UDR USART I/O Data Register 155
$0B ($2B) UCSRA RXC TXC UDRE FE DOR PE U2X MPCM 155
$0A ($2A) UCSRB RXCIE TXCIE UDRIE RXEN TXEN UCSZ2 RXB8 TXB8 156
$09 ($29) UBRRL USART Baud Rate Register Low Byte 159
$08 ($28) ACSR ACD ACBG ACO ACI ACIE ACIC ACIS1 ACIS0 164
$07 ($27) PORTE -- -- -- -- -- PORTE2 PORTE1 PORTE0 76
$06 ($26) DDRE -- -- -- -- -- DDE2 DDE1 DDE0 76
$05 ($25) PINE -- -- -- -- -- PINE2 PINE1 PINE0 76
$04 ($24) OSCCAL Oscillator Calibration Register 39
]]

mega8515_vec = [[
1 $000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog Reset
2 $001 INT0 External Interrupt Request 0
3 $002 INT1 External Interrupt Request 1
4 $003 TIMER1_CAPT Timer/Counter1 Capture Event
5 $004 TIMER1_COMPA Timer/Counter1 Compare Match A
6 $005 TIMER1_COMPB Timer/Counter1 Compare Match B
7 $006 TIMER1_OVF Timer/Counter1 Overflow
8 $007 TIMER0_OVF Timer/Counter0 Overflow
9 $008 SPI_STC Serial Transfer Complete
10 $009 USART_RXC USART Rx Complete
11 $00A USART_UDRE USART Data Register Empty
12 $00B USART_TXC USART Tx Complete
13 $00C ANA_COMP Analog Comparator
14 $00D INT2 External Interrupt Request 2
15 $00E TIMER0_COMP Timer/Counter0 Compare Match
16 $00F EE_READY EEPROM Ready
17 $010 SPM_READY Store Program memory Ready
]]

dofile 'target/HC08/string.lua'

function show_vectors(v, size)
vt = {}
name_max = 0  -- width of name field
v = v:gsub("%$(%x)", "0x%1")   -- replace $ with 0x to normalise
for num, addr, name, desc in v:gmatch("(%d+) (0x%x+) ([%w_]+) (.-)\n") do
   if desc ~= "Reserved" then
      table.insert(vt, { addr=addr/size, desc=desc, name=name })
   end
   if #name > name_max then name_max = #name end
   --print (addr, desc, name)
end

--print (name_max)

for _,vec in ipairs(vt) do
   -- print(string.format("#%02d  %05x  vec  %s  ( %s)",
   -- 0x10000-vec.addr, vec.addr, vec.name..(" "):rep(name_max-#vec.name), vec.desc))
   print(string.format("%04x vector  %s  ( %s)",
      vec.addr, vec.name..(" "):rep(name_max-#vec.name), vec.desc))
end
print()
end

function show_regs(s)
   s = s:gsub("%$(%x)", "0x%1")   -- replace $ with 0x to normalise
   local pat = "%((0x%x+)%) ([%w_/]+)" .. (" ([%w<-]+)"):rep(8)

-- (0x60) WDTCSR WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0 55
-- 0x3F (0x5F) SREG I T H S V N Z C 10

   local function show_line(l)
      -- skip whitespace-only lines or Reserved slots
      if l:match "^%s*$" or l:match "Reserved" then return end
      local addr, reg, b7, b6, b5, b4, b3, b2, b1, b0 = l:match(pat)
      if addr then
         print (string.format("%04x reg  %-8s | "..("%-7s "):rep(8),
                              addr, reg, b7, b6, b5, b4, b3, b2, b1, b0))
         return
      end

-- (0xC6) UDR0 USART I/O Data Register 193
      addr, reg, desc = l:match "%((0x%x+)%) ([%w_]+) (.+)"
      print (string.format("%04x reg  %-8s | %s", addr, reg, desc))
      -- print (string.format("! %s failed to match - need exactly 8 items after name",
      --       reg))
   end

   for l in s:lines() do
      show_line(l)
   end
   print()
end

function show_heading(s)
   print (string.format(
      "( Equates for ATMEGA%s. Extracted from the datasheet using Lua!)", s))
   print()
end

function show_part(name, vec, vecsize, reg)
   show_heading(name)
   show_vectors(vec, vecsize)
   show_regs(reg)
end

-- show_part(" (common)", common_vec, common_sfr)
show_part("164/324/644",  megax4_vec, 2, megax4_reg)
show_part("48/88/168/328", megax8_vec, 1, megax8_reg)
show_part("8515", mega8515_vec, 1, mega8515_reg)
