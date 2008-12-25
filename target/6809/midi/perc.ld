( perc.ld -- load file for general percussion )
( daf  10-feb-93 created )

forth

new  ( clear out target memory, reset pointers )

target

( these signatures should come *before* the kernel is loaded )

signature" Percussion FASTMIDI driver, v3.0. "
signature" PortA->Disp, PortB->Drivers, PortC->unused; "
signature" Via->Firq, 6850->Firq, PanelSwitch->Irq. "
signature" Copyright (c) 1992-94 David Frech and "
signature" Remmy Demmy Productions."

forth ld ::compiler:kernel

( kernel file leaves us in target compile mode )

forth ld ::midi:fast.perc
forth ld ::midi:fast.midi

