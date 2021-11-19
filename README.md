# muforth

Personal fork of https://github.com/nimblemachines/muforth

New users might to watch or clone this repository, as it actively tries
to identify and offer solutions for tripping points or omissions that
have not yet been incorporated into the master repository above.

A case in point as of this writing:

If you are a new user with a fresh factory stm32fx-discovery board and
you attempt to chat with the target using the appropriate default
target/ARM/board/ **in the master repository**, you're going to have
some weird moments with flash because they now load "special" example
code.  You can either reflash the board with that code OR you can just
use the target/ARM/board files clearly labelled with the word "factory"
in this repository.

Documentation for muforth is, at present, scattered between git logs and
code statements; improving this is at the top of everyone's list. 


# Supported platforms/architectures

Currently tested and working on the following systems/platforms:

* OpenBSD (i386, amd64)
* Linux (Archlinux, Gentoo, Debian, Alpine) (x86_64, aarch64)
* NixOS (x86_64)
* Termux (aarch64)
