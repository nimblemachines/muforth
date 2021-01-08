
# What is muforth?

muforth is a small, simple, fast, indirect-threaded code (ITC) Forth intended
for use as a cross-compiler for microcontrollers and other embedded devices.
It is written in C and its core is very portable. Because of its Forth nature,
it is naturally extensible, scriptable, and customizable.

It is very well-suited to interactive coding, debugging, and exploration, and
is a great tool for bringing up new hardware.

It has support – in varying degrees of completeness – for the following
architectures:

  * [ARMv5](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v5) (originally targeting an ARM AEB-1 board running an ARM7DI processor)
  * [ARMv6-m](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v6-m) (aka Cortex-M0/M0+)
  * [ARMv7-m](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v7-m) (aka Cortex-M3/M4)
  * [AVR](https://github.com/nimblemachines/muforth/tree/master/mu/target/AVR) (Atmel)
  * [HC08 and HCS08](https://github.com/nimblemachines/muforth/tree/master/mu/target/HC08) (Motorola/Freescale/NXP)
  * [MSP430](https://github.com/nimblemachines/muforth/tree/master/mu/target/MSP430) (TI)
  * [PIC18](https://github.com/nimblemachines/muforth/tree/master/mu/target/PIC18) (Microchip)
  * [RISC-V](https://github.com/nimblemachines/muforth/tree/master/mu/target/RISC-V) (initially targeting the SiFive FE310)


# Why yet another Forth?

I initially wrote muforth because I wanted to try out some implementation
ideas. Today there is very little that distinguishes muforth from fig-FORTH –
but the differences go rather deep.

Its implementation is no longer the point. Its sole reason for existing
is to be a cross/meta/target-compiler for _other_ Forths, and their
implementations are in no way tied to muforth's.

It’s [BSD
licensed](https://github.com/nimblemachines/muforth/blob/master/LICENSE), so do
what you want with it! I’d love to hear about innovative or unusual uses of
muforth.


# Starting points

[BUILDING](https://github.com/nimblemachines/muforth/blob/master/BUILDING)
will tell you how to build muforth. It's stupid simple.

Look in
[mu/target/](https://github.com/nimblemachines/muforth/tree/master/mu/target)
to find a target that interests you. There is generally a
`mu/target/<target-name>/build.mu4` that loads the cross-build environment. Use it as
an "index" to find the assembler, disassembler, meta-compiler, kernel, and
other key pieces of code.


# Documentation

Sadly, there isn't a lot of documentation right now. A good place to start is
to read the source.

The code – both the C code that implements the Forth virtual machine and the
muforth code that does everything else – is carefully and in some cases
extensively (even obsessively!) commented. Right now your best source of
documentation is the code! Luckily for you, there isn’t that much, so reading
it is actually possible. That’s part of the point of muforth; I want it to be a
[convivial tool](https://www.nimblemachines.com/convivial-tool/).

The heart of the system is the Forth code that muforth reads when it first
starts:
[mu/startup.mu4](https://github.com/nimblemachines/muforth/blob/master/mu/startup.mu4).
You’ll learn a lot by reading this!

[muforth.nimblemachines.com](https://muforth.nimblemachines.com/) is also,
finally, spreading its wings.


# Talks

In March 2008 I gave a [talk](https://vimeo.com/859408) about bootstrapping,
muforth, and [convivial tools](https://www.nimblemachines.com/convivial-tool/).

Warning: I wave my arms around a lot, and the audio and video quality isn’t
that great, but you might find it interesting, or at least amusing.

It’s also hard to see my slides. If you want to “follow along”, [download my
slides](https://github.com/nimblemachines/muforth/blob/master/talks/2008-mar-30-PNCA),
and use `less` to view them – ideally in a text window that is about 31 lines
high – like so:

```
less -G ~/muforth/talks/2008-mar-30-PNCA
```

Use `/^===` to get to the second page, and `n` to page forward after that.

[![March 2008 talk on Vimeo](https://i.vimeocdn.com/video/53151468)](https://vimeo.com/859408)

# Above all, enjoy!
