# muforth and .uf2 files

muforth now includes nascent support for generating uf2 files for the
drag and drop crowd. 

muforth/mu/target/ARM/board/picouf2.mu4 is a template file that sets
things up correctly so you can append your own code files with ld ...

### Example

```
$ cd muforth/mu
$ ./muforth -f target/ARM/board/picouf2.mu4 flash pico-uf2 bye > flashybits.uf2

$ ls flashybits.uf2
flashybits.uf2

$ file flashybits.uf2
flashybits.uf2: data

$ hexdump -C flashybits.uf2 | less
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
