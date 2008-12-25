( wwind.ld -- load file for woodwind )
( daf  14-jun-93 created )

forth

new  ( clear out target memory, reset pointers )

target

( these signatures should come *before* the kernel is loaded )

signature" Woodwind driver, v2.0. "
signature" PortA->Disp, PortB->Drivers, PortC->unused; "
signature" Via->Firq, 6850->Firq, PanelSwitch->Irq. "
signature" Copyright (c) 1992, 1993 David Frech and "
signature" Remmy Demmy Productions."

forth ld ::compiler:kernel

( kernel file leaves us in target compile mode )

forth ld ::midi:wwind
forth ld ::midi:midi.core

