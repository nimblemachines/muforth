( slave.ld -- 02 feb 94 )

forth

new  ( clear out target memory, reset pointers )

target

( these signatures should come *before* the kernel is loaded )

signature" PHFFT! MIDI Slave, v2.2. "
signature" PortA->Display, PortB->Perc (ch #1), "
signature" PortC->Sustain (ch #2); "
signature" Via->Firq, 6850->Firq, Irq unused; "
signature" 600 usec clock. "
signature" Copyright (c) 1992, 1993, 1994 David Frech and "
signature" Remmy Demmy Productions."

forth ld ::midi:kernel

( kernel file leaves us in target compile mode )

forth ld ::PHFFT!:slave.main
forth ld ::midi:fast.midi

