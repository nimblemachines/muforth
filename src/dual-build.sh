#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# Build both 32-bit and 64-bit versions of muforth, in case we want to
# deploy on another machine. But this is *not* a cross-build! The target
# machine has to be the same OS as the build machine.

# Make 32-bit version
./configure.sh force32
make
mv ../mu/muforth ../mu/muforth32

# Make 64-bit version
./configure.sh force64
make
mv ../mu/muforth ../mu/muforth64

# Make standard version
./configure.sh
make