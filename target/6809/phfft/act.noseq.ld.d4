( act.noseq.ld )

forth

new  ( clear out target memory, reset pointers )

target

( these signatures should come *before* the kernel is loaded )

signature" PHFFT! Wheels v1.1. "
signature" Wheel1: A->ca1&ca2, B->pa7; "
signature" Wheel2: A->cb1&cb2, B->pa7; "
signature" No seqencer support. "
signature" Via->Firq, Irq unused. "
signature" Copyright (c) 1992-1997 David Frech and "
signature" Remmy Demmy Productions."

forth ld ::compiler:kernel

( kernel file leaves us in target compile mode )

forth ld ::PHFFT!:act
forth ld ::PHFFT!:noseq

