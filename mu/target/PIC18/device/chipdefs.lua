-- This file is part of muforth: https://muforth.nimblemachines.com/

-- Copyright (c) 2002-2019 David Frech. (Read the LICENSE for details.)

-- Equates for several PIC18 variants, "scraped" from Microchip datasheets.
-- From here we generate muforth code!

-- Be careful of pasted in endashes and ligatures!
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

-- Also beware copyright and trademark characters.
-- :%s/\%u2122//g   ( tm)

-- 1xK50 chip defs

-- NOTE: Had to change SSPMASK to SSPMSK, to match the reg bits string.
pic_1xk50_reg_addrs = [[
FFFh TOSU FD7h TMR0H FAFh SPBRG F87h --(2) F5Fh UEIR
FFEh TOSH FD6h TMR0L FAEh RCREG F86h --(2) F5Eh UFRMH
FFDh TOSL FD5h T0CON FADh TXREG F85h --(2) F5Dh UFRML
FFCh STKPTR FD4h --(2) FACh TXSTA F84h --(2) F5Ch UADDR
FFBh PCLATU FD3h OSCCON FABh RCSTA F83h --(2) F5Bh UEIE
FFAh PCLATH FD2h OSCCON2 FAAh -- F82h PORTC F5Ah UEP7
FF9h PCL FD1h WDTCON FA9h EEADR F81h PORTB F59h UEP6
FF8h TBLPTRU FD0h RCON FA8h EEDATA F80h PORTA F58h UEP5
FF7h TBLPTRH FCFh TMR1H FA7h EECON2(1) F7Fh ANSELH F57h UEP4
FF6h TBLPTRL FCEh TMR1L FA6h EECON1 F7Eh ANSEL F56h UEP3
FF5h TABLAT FCDh T1CON FA5h --(2) F7Dh --(2) F55h UEP2
FF4h PRODH FCCh TMR2 FA4h --(2) F7Ch --(2) F54h UEP1
FF3h PRODL FCBh PR2 FA3h --(2) F7Bh --(2) F53h UEP0
FF2h INTCON FCAh T2CON FA2h IPR2 F7Ah IOCB
FF1h INTCON2 FC9h SSPBUF FA1h PIR2 F79h IOCA
FF0h INTCON3 FC8h SSPADD FA0h PIE2 F78h WPUB
FEFh INDF0(1) FC7h SSPSTAT F9Fh IPR1 F77h WPUA
FEEh POSTINC0(1) FC6h SSPCON1 F9Eh PIR1 F76h SLRCON
FEDh POSTDEC0(1) FC5h SSPCON2 F9Dh PIE1 F75h --(2)
FECh PREINC0(1) FC4h ADRESH F9Ch --(2) F74h --(2)
FEBh PLUSW0(1) FC3h ADRESL F9Bh OSCTUNE F73h --(2)
FEAh FSR0H FC2h ADCON0 F9Ah --(2) F72h --(2)
FE9h FSR0L FC1h ADCON1 F99h --(2) F71h --(2)
FE8h WREG FC0h ADCON2 F98h --(2) F70h --(2)
FE7h INDF1(1) FBFh CCPR1H F97h --(2) F6Fh SSPMSK
FE6h POSTINC1(1) FBEh CCPR1L F96h --(2) F6Eh --(2)
FE5h POSTDEC1(1) FBDh CCP1CON F95h --(2) F6Dh CM1CON0
FE4h PREINC1(1) FBCh REFCON2 F94h TRISC F6Ch CM2CON1
FE3h PLUSW1(1) FBBh REFCON1 F93h TRISB F6Bh CM2CON0
FE2h FSR1H FBAh REFCON0 F92h TRISA F6Ah --(2)
FE1h FSR1L FB9h PSTRCON F91h --(2) F69h SRCON1
FE0h BSR FB8h BAUDCON F90h --(2) F68h SRCON0
FDFh INDF2(1) FB7h PWM1CON F8Fh --(2) F67h --(2)
FDEh POSTINC2(1) FB6h ECCP1AS F8Eh --(2) F66h --(2)
FDDh POSTDEC2(1) FB5h --(2) F8Dh --(2) F65h --(2)
FDCh PREINC2(1) FB4h --(2) F8Ch --(2) F64h UCON
FDBh PLUSW2(1) FB3h TMR3H F8Bh LATC F63h USTAT
FDAh FSR2H FB2h TMR3L F8Ah LATB F62h UIR
FD9h FSR2L FB1h T3CON F89h LATA F61h UCFG
FD8h STATUS FB0h SPBRGH F88h --(2) F60h UIE
]]

pic_1xk50_reg_bits = [[
TOSU -- -- -- Top-of-Stack Upper Byte (TOS<20:16>) ---0 0000 285, 30
TOSH Top-of-Stack, High Byte (TOS<15:8>) 0000 0000 285, 30
TOSL Top-of-Stack, Low Byte (TOS<7:0>) 0000 0000 285, 30
STKPTR STKFUL STKUNF -- SP4 SP3 SP2 SP1 SP0 00-0 0000 285, 31
PCLATU -- -- -- Holding Register for PC<20:16> ---0 0000 285, 30
PCLATH Holding Register for PC<15:8> 0000 0000 285, 30
PCL PC, Low Byte (PC<7:0>) 0000 0000 285, 30
TBLPTRU -- -- -- Program Memory Table Pointer Upper Byte (TBLPTR<20:16>) ---0 0000 285, 54
TBLPTRH Program Memory Table Pointer, High Byte (TBLPTR<15:8>) 0000 0000 285, 54
TBLPTRL Program Memory Table Pointer, Low Byte (TBLPTR<7:0>) 0000 0000 285, 54
TABLAT Program Memory Table Latch 0000 0000 285, 54
PRODH Product Register, High Byte xxxx xxxx 285, 65
PRODL Product Register, Low Byte xxxx xxxx 285, 65
INTCON GIE/GIEH PEIE/GIEL TMR0IE INT0IE RABIE TMR0IF INT0IF RABIF 0000 000x 285, 70
INTCON2 RABPU INTEDG0 INTEDG1 INTEDG2 -- TMR0IP -- RABIP 1111 -1-1 285, 71
INTCON3 INT2IP INT1IP -- INT2IE INT1IE -- INT2IF INT1IF 11-0 0-00 285, 72
INDF0 Uses contents of FSR0 to address data memory -- value of FSR0 not changed (not a physical register) N/A 285, 47
POSTINC0 Uses contents of FSR0 to address data memory -- value of FSR0 post-incremented (not a physical register) N/A 285, 47
POSTDEC0 Uses contents of FSR0 to address data memory -- value of FSR0 post-decremented (not a physical register) N/A 285, 47
PREINC0 Uses contents of FSR0 to address data memory -- value of FSR0 pre-incremented (not a physical register) N/A 285, 47
PLUSW0 Uses contents of FSR0 to address data memory -- value of FSR0 pre-incremented (not a physical register) -- value of FSR0 offset by W N/A 285, 47
FSR0H -- -- -- -- Indirect Data Memory Address Pointer 0, High Byte ---- 0000 285, 47
FSR0L Indirect Data Memory Address Pointer 0, Low Byte xxxx xxxx 285, 47
WREG Working Register xxxx xxxx 285
INDF1 Uses contents of FSR1 to address data memory -- value of FSR1 not changed (not a physical register) N/A 285, 47
POSTINC1 Uses contents of FSR1 to address data memory -- value of FSR1 post-incremented (not a physical register) N/A 285, 47
POSTDEC1 Uses contents of FSR1 to address data memory -- value of FSR1 post-decremented (not a physical register) N/A 285, 47
PREINC1 Uses contents of FSR1 to address data memory -- value of FSR1 pre-incremented (not a physical register) N/A 285, 47
PLUSW1 Uses contents of FSR1 to address data memory -- value of FSR1 pre-incremented (not a physical register) -- value of FSR1 offset by W N/A 285, 47
FSR1H -- -- -- -- Indirect Data Memory Address Pointer 1, High Byte ---- 0000 286, 47
FSR1L Indirect Data Memory Address Pointer 1, Low Byte xxxx xxxx 286, 47
BSR -- -- -- -- Bank Select Register ---- 0000 286, 35
INDF2 Uses contents of FSR2 to address data memory -- value of FSR2 not changed (not a physical register) N/A 286, 47
POSTINC2 Uses contents of FSR2 to address data memory -- value of FSR2 post-incremented (not a physical register) N/A 286, 47
POSTDEC2 Uses contents of FSR2 to address data memory -- value of FSR2 post-decremented (not a physical register) N/A 286, 47
PREINC2 Uses contents of FSR2 to address data memory -- value of FSR2 pre-incremented (not a physical register) N/A 286, 47
PLUSW2 Uses contents of FSR2 to address data memory -- value of FSR2 pre-incremented (not a physical register) -- value of FSR2 offset by W N/A 286, 47
FSR2H -- -- -- -- Indirect Data Memory Address Pointer 2, High Byte ---- 0000 286, 47
FSR2L Indirect Data Memory Address Pointer 2, Low Byte xxxx xxxx 286, 47
STATUS -- -- -- N OV Z DC C ---x xxxx 286, 45
TMR0H Timer0 Register, High Byte 0000 0000 286, 103
TMR0L Timer0 Register, Low Byte xxxx xxxx 286, 103
T0CON TMR0ON T08BIT T0CS T0SE PSA T0PS2 T0PS1 T0PS0 1111 1111 286, 101
OSCCON IDLEN IRCF2 IRCF1 IRCF0 OSTS IOSF SCS1 SCS0 0011 qq00 286, 20
OSCCON2 -- -- -- -- -- PRI_SD HFIOFL LFIOFS ---- -10x 286, 21
WDTCON -- -- -- -- -- -- -- SWDTEN --- ---0 286, 303
RCON IPEN SBOREN(1) -- RI TO PD POR BOR 0q-1 11q0 277, 284, 79
TMR1H Timer1 Register, High Byte xxxx xxxx 286, 110
TMR1L Timer1 Register, Low Bytes xxxx xxxx 286, 110
T1CON RD16 T1RUN T1CKPS1 T1CKPS0 T1OSCEN T1SYNC TMR1CS TMR1ON 0000 0000 286, 105
TMR2 Timer2 Register 0000 0000 286, 112
PR2 Timer2 Period Register 1111 1111 286, 112
T2CON -- T2OUTPS3 T2OUTPS2 T2OUTPS1 T2OUTPS0 TMR2ON T2CKPS1 T2CKPS0 -000 0000 286, 111
SSPBUF SSP Receive Buffer/Transmit Register xxxx xxxx 286, 143, 144
SSPADD SSP Address Register in I2C Slave Mode. SSP Baud Rate Reload Register in I2C Master Mode. 0000 0000 286, 144
SSPSTAT SMP CKE D/A P S R/W UA BF 0000 0000 286, 137, 146
SSPCON1 WCOL SSPOV SSPEN CKP SSPM3 SSPM2 SSPM1 SSPM0 0000 0000 286, 137, 146
SSPCON2 GCEN ACKSTAT ACKDT ACKEN RCEN PEN RSEN SEN 0000 0000 286, 147
ADRESH A/D Result Register, High Byte xxxx xxxx 287, 221
ADRESL A/D Result Register, Low Byte xxxx xxxx 287, 221
ADCON0 -- -- CHS3 CHS2 CHS1 CHS0 GO/DONE ADON --00 0000 287, 215
ADCON1 -- -- -- -- PVCFG1 PVCFG0 NVCFG1 NVCFG0 ---- 0000 287, 216
ADCON2 ADFM -- ACQT2 ACQT1 ACQT0 ADCS2 ADCS1 ADCS0 0-00 0000 287, 217
CCPR1H Capture/Compare/PWM Register 1, High Byte xxxx xxxx 287, 138
CCPR1L Capture/Compare/PWM Register 1, Low Byte xxxx xxxx 287, 138
CCP1CON P1M1 P1M0 DC1B1 DC1B0 CCP1M3 CCP1M2 CCP1M1 CCP1M0 0000 0000 287, 117
REFCON2 -- -- -- DAC1R4 DAC1R3 DAC1R2 DAC1R1 DAC1R0 ---0 0000 287, 248
REFCON1 D1EN D1LPS DAC1OE --- D1PSS1 D1PSS0 -- D1NSS 000- 00-0 287, 248
REFCON0 FVR1EN FVR1ST FVR1S1 FVR1S0 -- -- -- -- 0001 00-- 287, 247
PSTRCON -- -- -- STRSYNC STRD STRC STRB STRA ---0 0001 287, 134
BAUDCON ABDOVF RCIDL DTRXP CKTXP BRG16 -- WUE ABDEN 0100 0-00 287, 192
PWM1CON PRSEN PDC6 PDC5 PDC4 PDC3 PDC2 PDC1 PDC0 0000 0000 287, 133
ECCP1AS ECCPASE ECCPAS2 ECCPAS1 ECCPAS0 PSSAC1 PSSAC0 PSSBD1 PSSBD0 0000 0000 287, 129
TMR3H Timer3 Register, High Byte xxxx xxxx 287, 115
TMR3L Timer3 Register, Low Byte xxxx xxxx 287, 115
T3CON RD16 -- T3CKPS1 T3CKPS0 T3CCP1 T3SYNC TMR3CS TMR3ON 0-00 0000 287, 113
SPBRGH EUSART Baud Rate Generator Register, High Byte 0000 0000 287, 181
SPBRG EUSART Baud Rate Generator Register, Low Byte 0000 0000 287, 181
RCREG EUSART Receive Register 0000 0000 287, 182
TXREG EUSART Transmit Register 0000 0000 287, 181
TXSTA CSRC TX9 TXEN SYNC SENDB BRGH TRMT TX9D 0000 0010 287, 190
RCSTA SPEN RX9 SREN CREN ADDEN FERR OERR RX9D 0000 000x 287, 191
EEADR EEADR7 EEADR6 EEADR5 EEADR4 EEADR3 EEADR2 EEADR1 EEADR0 0000 0000 287, 52, 61
EEDATA EEPROM Data Register 0000 0000 287, 52, 61
EECON2 EEPROM Control Register 2 (not a physical register) 0000 0000 287, 52, 61
EECON1 EEPGD CFGS -- FREE WRERR WREN WR RD xx-0 x000 287, 53, 61
IPR2 OSCFIP C1IP C2IP EEIP BCLIP USBIP TMR3IP -- 1111 111- 288, 78
PIR2 OSCFIF C1IF C2IF EEIF BCLIF USBIF TMR3IF -- 0000 000- 288, 74
PIE2 OSCFIE C1IE C2IE EEIE BCLIE USBIE TMR3IE -- 0000 000- 288, 76
IPR1 -- ADIP RCIP TXIP SSPIP CCP1IP TMR2IP TMR1IP -111 1111 288, 77
PIR1 -- ADIF RCIF TXIF SSPIF CCP1IF TMR2IF TMR1IF -000 0000 288, 73
PIE1 -- ADIE RCIE TXIE SSPIE CCP1IE TMR2IE TMR1IE -000 0000 288, 75
OSCTUNE INTSRC SPLLEN TUN5 TUN4 TUN3 TUN2 TUN1 TUN0 0000 0000 22, 288
TRISC TRISC7 TRISC6 TRISC5 TRISC4 TRISC3 TRISC2 TRISC1 TRISC0 1111 1111 288, 94
TRISB TRISB7 TRISB6 TRISB5 TRISB4 -- -- -- -- 1111 ---- 288, 89
TRISA -- -- TRISA5 TRISA4 -- -- -- -- --11 ---- 288, 83
LATC LATC7 LATC6 LATC5 LATC4 LATC3 LATC2 LATC1 LATC0 xxxx xxxx 288, 94
LATB LATB7 LATB6 LATB5 LATB4 -- -- -- -- xxxx ---- 288, 89
LATA -- -- LATA5 LATA4 -- -- -- -- --xx ---- 288, 83
PORTC RC7 RC6 RC5 RC4 RC3 RC2 RC1 RC0 xxxx xxxx 288, 94
PORTB RB7 RB6 RB5 RB4 -- -- -- -- xxxx ---- 288, 89
PORTA -- -- RA5 RA4 RA3(2) -- RA1(3) RA0(3) --xx x-xx 288, 83
ANSELH -- -- -- -- ANS11 ANS10 ANS9 ANS8 ---- 1111 288, 99
ANSEL ANS7 ANS6 ANS5 ANS4 ANS3 -- -- -- 1111 1--- 288, 98
IOCB IOCB7 IOCB6 IOCB5 IOCB4 -- -- -- -- 0000 ---- 288, 89
IOCA -- -- IOCA5 IOCA4 IOCA3 -- IOCA1 IOCA0 --00 0-00 288, 83
WPUB WPUB7 WPUB6 WPUB5 WPUB4 -- -- -- -- 1111 ---- 288, 89
WPUA -- -- WPUA5 WPUA4 WPUA3 -- -- -- --11 1--- 285, 89
SLRCON -- -- -- -- -- SLRC SLRB SLRA ---- -111 288, 100
SSPMSK MSK7 MSK6 MSK5 MSK4 MSK3 MSK2 MSK1 MSK0 1111 1111 288, 154
CM1CON0 C1ON C1OUT C1OE C1POL C1SP C1R C1CH1 C1CH0 0000 1000 288, 229
CM2CON1 MC1OUT MC2OUT C1RSEL C2RSEL C1HYS C2HYS C1SYNC C2SYNC 0000 0000 288, 230
CM2CON0 C2ON C2OUT C2OE C2POL C2SP C2R C2CH1 C2CH0 0000 1000 288, 230
SRCON1 SRSPE SRSCKE SRSC2E SRSC1E SRRPE SRRCKE SRRC2E SRRC1E 0000 0000 288, 243
SRCON0 SRLEN SRCLK2 SRCLK1 SRCLK0 SRQEN SRNQEN SRPS SRPR 0000 0000 288, 242
UCON -- PPBRST SE0 PKTDIS USBEN RESUME SUSPND -- -0x0 000- 288, 252
USTAT -- ENDP3 ENDP2 ENDP1 ENDP0 DIR PPBI -- -xxx xxx- 289, 256
UIR -- SOFIF STALLIF IDLEIF TRNIF ACTVIF UERRIF URSTIF -000 0000 289, 266
UCFG UTEYE -- -- UPUEN -- FSEN PPB1 PPB0 0--0 -000 289, 254
UIE -- SOFIE STALLIE IDLEIE TRNIE ACTVIE UERRIE URSTIE -000 0000 289, 268
UEIR BTSEF -- -- BTOEF DFN8EF CRC16EF CRC5EF PIDEF 0--0 0000 289, 269
UFRMH -- -- -- -- -- FRM10 FRM9 FRM8 ---- -xxx 289, 252
UFRML FRM7 FRM6 FRM5 FRM4 FRM3 FRM2 FRM1 FRM0 xxxx xxxx 289, 252
UADDR -- ADDR6 ADDR5 ADDR4 ADDR3 ADDR2 ADDR1 ADDR0 -000 0000 289, 258
UEIE BTSEE -- -- BTOEE DFN8EE CRC16EE CRC5EE PIDEE 0--0 0000 289, 270
UEP7 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP6 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP5 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP4 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP3 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP2 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP1 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 289, 257
UEP0 -- -- -- EPHSHK EPCONDIS EPOUTEN EPINEN EPSTALL ---0 0000 285, 257
]]

pic_1xk50_configs = [[
300000h CONFIG1L -- -- USBDIV CPUDIV1 CPUDIV0 -- -- -- --00 0---
300001h CONFIG1H IESO FCMEN PCLKEN PLLEN FOSC3 FOSC2 FOSC1 FOSC0 0010 0111
300002h CONFIG2L -- -- -- BORV1 BORV0 BOREN1 BOREN0 PWRTEN ---1 1111
300003h CONFIG2H -- -- -- WDTPS3 WDTPS2 WDTPS1 WDTPS0 WDTEN ---1 1111
300005h CONFIG3H MCLRE -- -- -- HFOFST -- -- -- 1--- 1---
300006h CONFIG4L BKBUG(2) ENHCPU -- -- BBSIZ LVP -- STVREN -0-- 01-1
300008h CONFIG5L -- -- -- -- -- -- CP1 CP0 ---- --11
300009h CONFIG5H CPD CPB -- -- -- -- -- -- 11-- ----
30000Ah CONFIG6L -- -- -- -- -- -- WRT1 WRT0 ---- --11
30000Bh CONFIG6H WRTD WRTB WRTC -- -- -- -- -- 111- ----
30000Ch CONFIG7L -- -- -- -- -- -- EBTR1 EBTR0 ---- --11
30000Dh CONFIG7H -- EBTRB -- -- -- -- -- -- -1-- ----
3FFFFEh DEVID1(1) DEV2 DEV1 DEV0 REV4 REV3 REV2 REV1 REV0 qqqq qqqq(1)
3FFFFFh DEVID2(1) DEV10 DEV9 DEV8 DEV7 DEV6 DEV5 DEV4 DEV3 0000 1100
]]

dofile 'target/S08/device/string.lua'

-- S08
-- UNUSED currently for PIC18
function show_vectors(v)
    vt = {}
    name_max = 0  -- width of name field
    for addr, desc, name in v:gmatch("(0x%x+):0?x?%x+ ([%w ]-) ([%w%-]+)%s-\n") do
       if desc ~= "Reserved" then
          table.insert(vt, 1, { addr=addr, desc=desc, name=name })
       end
       if #name > name_max then name_max = #name end
       --print (addr, desc, name)
    end

    --print (name_max)

    for _,vec in ipairs(vt) do
       -- print(string.format("#%02d  %05x  vec  %s  ( %s)",
       -- 0x10000-vec.addr, vec.addr, vec.name..(" "):rep(name_max-#vec.name), vec.desc))
       print(string.format("%05x vector  %s  ( %s)",
          vec.addr, vec.name..(" "):rep(name_max-#vec.name), vec.desc))
    end
    print()
end

function show_regs(reg_addrs, reg_bits, configs)
    local reg2addr = {}

-- Some lines have five reg/addr pairs; some have four
-- FFFh TOSU FD7h TMR0H FAFh SPBRG F87h --(2) F5Fh UEIR
-- FD8h STATUS FB0h SPBRGH F88h --(2) F60h UIE
    local function read_addrs(l)
        if l:match "^%s*$" then return end    -- skip whitespace-only lines
        local firstpair = "(%x+)h ([%w_-]+)"
        local pair = " " .. firstpair
        local a1, r1, a2, r2, a3, r3, a4, r4, a5, r5 =
            l:match(firstpair .. pair:rep(4))
        if a1 then
            -- print(a1, r1, a2, r2, a3, r3, a4, r4, a5, r5)
            reg2addr[r1] = a1
            reg2addr[r2] = a2
            reg2addr[r3] = a3
            reg2addr[r4] = a4
            reg2addr[r5] = a5
        else
            a1, r1, a2, r2, a3, r3, a4, r4, a5, r5 =
                l:match(firstpair .. (pair):rep(3))
            if a1 then
                -- print(a1, r1, a2, r2, a3, r3, a4, r4)
                reg2addr[r1] = a1
                reg2addr[r2] = a2
                reg2addr[r3] = a3
                reg2addr[r4] = a4
            else
                print "Something went wrong reading the reg addresses"
            end
        end
    end

    local function addr(reg)
        return "0x" .. reg2addr[reg]
    end

-- Example of register definitions with 8 bit names following:
-- EEADR EEADR7 EEADR6 EEADR5 EEADR4 EEADR3 EEADR2 EEADR1 EEADR0 0000 0000 287, 52, 61
-- EECON1 EEPGD CFGS -- FREE WRERR WREN WR RD xx-0 x000 287, 53, 61

-- Examples that don't have 8 bit names:
-- EEDATA EEPROM Data Register 0000 0000 287, 52, 61
-- EECON2 EEPROM Control Register 2 (not a physical register) 0000 0000 287, 52, 61

    local function show_reg(l)
        if l:match "^%s*$" then return end    -- skip whitespace-only lines
        local pat = "([%w_]+)" .. (" ([%u%d/_-]+)"):rep(8)
        local reg, b7, b6, b5, b4, b3, b2, b1, b0 = l:match(pat)
        if reg then
            print (string.format("%04x reg  %-8s | "..("%-7s "):rep(8),
                                 addr(reg), reg, b7, b6, b5, b4, b3, b2, b1, b0))
            return
        end

        reg, desc = l:match "([%w_]+) (.+)"
        print (string.format("%04x reg  %-8s | %s", addr(reg), reg, desc))
    end

-- 300000h CONFIG1L -- -- USBDIV CPUDIV1 CPUDIV0 -- -- -- --00 0---
    local function show_config(l)
        if l:match "^%s*$" then return end    -- skip whitespace-only lines
        local pat = "(%x+)h ([%w_]+)" .. (" ([%u%d/_-]+)"):rep(8)
        local addr, reg, b7, b6, b5, b4, b3, b2, b1, b0 = l:match(pat)
        if addr then
            print (string.format("%06x reg  %-8s | "..("%-7s "):rep(8),
                                 "0x"..addr, reg, b7, b6, b5, b4, b3, b2, b1, b0))
            return
        end

        print("Failed to match config: ", l)
    end

    local function drop_footnotes(s)
        s = s:gsub("%(%d+%)", "")
        return s
    end

    for l in reg_addrs:lines() do
      read_addrs(drop_footnotes(l))
    end
    for l in reg_bits:lines() do
      show_reg(drop_footnotes(l))
    end
    print()
    for l in configs:lines() do
      show_config(drop_footnotes(l))
    end
end

function show_heading(s)
    print (string.format(
        "( Equates for PIC18F%s. Extracted from the datasheet using Lua!)", s))
    print()
end

function show_part(name, reg_addrs, reg_bits, configs)
    show_heading(name)
    show_regs(reg_addrs, reg_bits, configs)
end

show_part("1xK50", pic_1xk50_reg_addrs, pic_1xk50_reg_bits, pic_1xk50_configs)
