## When You Just Can't @flash Flash

Maybe you know this, maybe you don't.

**Embedded ARM always has a copy of flash starting at address 0.  You can always
dump from 0.**

For example, muforth as of this writing is in the very nascent stages of supporting
the RaspberryPi Pico, and we can't ```@flash du``` etc, but my goodness, who cares. ```0 du``` works 
just fine. You can du to about 1fe0; if you go further, you segfault and drop a core for Reasons 
(not ready for prime time reasons.)
```
cd muforth/mu
./muforth -f target/ARM/board/rasberry-pi-pico.mu4
pico
0 1fe0 read-image
save-image pico-zero.img
bye
```

Then do whatever the hell you want with it; strings, hexdump, [radare:](https://radare.org/n/)
```
r2 -a arm -m 0x0 -b 16 pico-zero.img
```
Go. To. Town.

This trick isn't specific to muforth, there's a lot of other tools out there you can use to [extract
the firmware off an embedded ARM device.](https://youtube.com/watch?v=GX8-K4TssjY)
