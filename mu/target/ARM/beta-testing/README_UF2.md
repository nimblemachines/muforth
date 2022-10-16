# muforth and .uf2 files

muforth now includes nascent support for generating uf2 files for the
drag and drop crowd:

```
$ cd muforth/mu
$ ./muforth -f target/ARM/board/raspberry-pi-pico.mu4 pico pico-uf2 > test.uf2
muforth/64 (4e66ad5e) 2022-oct-14 13:49 (https://muforth.nimblemachines.com/)
Copyright (c) 2002-2022 David Frech (read the LICENSE for details)

Type 'settings' to see a few of muforth's tweakable behaviours.

(( Raspberry Pi Pico board 
(( RP2040 chip 
(( RP2040 equates ))
(( ARM Cortex-M0+ processor 
(( ARM Cortex-M core equates (v6-M and v7-M) ))
(( Target endianness ))
(( Meta-compiler (chains and token consumers) ))
(( ARM v6-M memory image ))
(( Reading Motorola S-record (S19) files ))
(( Intel hex file support (reading and writing) h* again.  hh* again.  ))
(( UF2 file format ))
(( ARM v6-M Thumb2 disassembler ))
(( ARM v6-M Thumb2 assembler 4# again.  00r again.  i8 again.  sh-imm again.  ))
(( ARM v6-M meta-compiler (main) ; again.  ))
(( ARM v6-M interaction dumps again.  ))
(( Flash programming ))))))
(( Rasberry Pi PICOBOOT support ))
(( ARM v6-M Forth kernel (ITC) ))
(( Raspberry Pi Pico QSPI experiments (SIO mode) ))))
(( Bit Tests ))
PICOBOOT v2 
      SP        RP        IP
2003ff7c  00000000  00000000* 
bye

$ ls test.uf2
test.uf2

$ file test.uf2
test.uf2: data

$ hexdump -C test.uf2 | less
00000000  55 46 32 0a 57 51 5d 9e  00 20 00 00 00 00 00 20  |UF2.WQ].. ..... |
00000010  00 01 00 00 00 00 00 00  20 00 00 00 56 ff 8b e4  |........ ...V...|
00000020  00 00 00 00 00 68 80 b4  07 46 01 ce 02 c8 08 47  |.....h...F.....G|
00000030  80 b4 07 46 70 46 01 38  04 3d 2e 60 06 46 01 ce  |...FpF.8.=.`.F..|
00000040  02 c8 08 47 29 00 00 20  40 cd 01 ce 02 c8 08 47  |...G).. @......G|
00000050  2b 00 00 20 39 00 00 20  01 ce e4 e7 41 00 00 20  |+.. 9.. ....A.. |
...
```

NB: Your .uf2 will look different, as the above included some current testing code.

But wait! How the hell do you know this has worked? We can't even read
from flash, and if we can't read from flash we can't execute code!

It's called a cliffhanger ...
