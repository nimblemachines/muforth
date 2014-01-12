#/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# So that we always have an accurate git commit available to Forth code,
# after checkout or commit generate a muforth file that defines the current
# commit.

echo "Generating commit.mu4"

sha=`git rev-parse --verify HEAD`

echo ": muforth-commit  \" $sha\" ;" > commit.mu4
