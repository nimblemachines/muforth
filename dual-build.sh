#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2013 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# Build both 32-bit and 64-bit versions of muforth, in case we want to
# deplay on another machine. But this is *not* a cross-build! The target
# machine has to be the same OS as the build machine.

# Make 32-bit version
./configure.sh force32
make
mv muforth muforth32

# Make 64-bit version
./configure.sh force64
make
mv muforth muforth64

# Make standard version
./configure.sh
make
