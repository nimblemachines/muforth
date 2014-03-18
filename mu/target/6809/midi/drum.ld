( drum.ld -- load file for drums/lights )
( daf  9-feb-93 created )

forth

new  ( clear out target memory, reset pointers )

tt

( these signatures should come *before* the kernel is loaded )

signature" Drums/Lights for On The Boards. "
signature" PortA->Disp, PortB->Drums, PortC->Lights; "
signature" Via->Firq, 6850->Firq, PanelSwitch->Irq. "
signature" Copyright (c) 1992, 1993 David Frech and "
signature" Remmy Demmy Productions."

forth ld ::compiler:kernel

( kernel file leaves us in target compile mode )

forth ld drums
forth ld midi.core

