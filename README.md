
# What is muforth?

muforth is a small, simple, fast, indirect-threaded code (ITC) Forth intended
for use as a cross-compiler for microcontrollers and other embedded devices.
It is written in C and its core is very portable. Because of its Forth nature,
it is naturally extensible, scriptable, and customizable.

It is very well-suited to interactive coding, debugging, and exploration, and
is a great tool for bringing up new hardware.

It has support - in varying degrees of completeness - for the following
architectures:

  * [ARMv5](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v5)
    (originally targeting an ARM AEB-1 board running an ARM7DI processor)
  * [ARMv6-m](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v6-m) (aka Cortex-M0/M0+) and [ARMv7-m](https://github.com/nimblemachines/muforth/tree/master/mu/target/ARM/v7-m) (aka Cortex-M3/M4)
  * [Atmel AVR](https://github.com/nimblemachines/muforth/tree/master/mu/target/AVR)
  * [Motorola/Freescale/NXP HC08 and HCS08](https://github.com/nimblemachines/muforth/tree/master/mu/target/HC08)
  * [RISC-V](https://github.com/nimblemachines/muforth/tree/master/mu/target/RISC-V)
    (initially targeting the SiFive FE310)
  * [TI MSP430](https://github.com/nimblemachines/muforth/tree/master/mu/target/MSP430)


# Why yet another Forth?

I initially wrote muforth because I wanted to try out some implementation
ideas. Today there is very little that distinguishes muforth from fig-FORTH -
but the differences go rather deep.

Its implementation is no longer the point. Its sole reason for existing
is to be a cross/meta/target-compiler for _other_ Forths, and their
implementations are in no way tied to muforth's.


# Starting points

[BUILDING](https://github.com/nimblemachines/muforth/blob/master/BUILDING)
will tell you how to build muforth. It's stupid simple.

Sadly, there isn't a lot of documentation right now. A good place to start is
to read the source. It's generally well-commented, and is intended to be read
and understood. mu/startup.mu4 - which is read and executed every time muforth
starts - is the heart of the system.

Look in
[mu/target/](https://github.com/nimblemachines/muforth/tree/master/mu/target)
to find a target that interests you. There is generally a
mu/target/<target>/build.mu4 that loads the cross-build environment. Use it as
an "index" to find the assembler, disassembler, meta-compiler, kernel, and
other key pieces of code.

[muforth.nimblemachines.com](http://muforth.nimblemachines.com/) is also,
finally, spreading its wings.

Above all, enjoy!
