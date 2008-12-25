( swiss.perc.ld -- load file for general percussion )
( daf  10-feb-93 created )

forth

new  ( clear out target memory, reset pointers )

tt

( these signatures should come *before* the kernel is loaded )
( this is a special version to make the swiss percussion )
( setup midi compatible.  It makes the display port, portA, )
( an input port so it doesn't drive the outputs of the ls151 )
( in the keyboard interface. It also ignores ca2, the 'go' )
( switch for the sequencer.  And, it starts in channel 1 )
( instead of omni mode. )

signature" Special Swiss Percussion MIDI driver, v169. "
signature" PortA->Input, PortB->Drivers, PortC->unused; "
signature" Via->Firq, 6850->Firq, PanelSwitch->Nothing. "
signature" Copyright (c) 1992, 1993 David Frech and "
signature" Remmy Demmy Productions."

forth ld kernel

( kernel file leaves us in target compile mode )

forth ld perc
forth ld swiss.midi.core

