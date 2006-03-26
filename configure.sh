#!/bin/sh

# $Id$

# configures which make to use
if make --version 2> /dev/null | grep -q "GNU Make"; then
cat <<EOF
Looks like you're running GNU make. I'm going to create a compatible
makefile for you.

EOF
  sed -E -f scripts/make.sed -f scripts/gnu.make.sed Makefile.in > Makefile
else
  cat <<EOF
Looks like you're running a non-GNU (perhaps BSD?) make. I'm going to
assume it's a BSD make and create a makefile for you. If the build
fails, try running "gmake" instead of "make".

EOF
  sed -E -f scripts/make.sed Makefile.in > Makefile
fi

cat <<EOF
You can build a native x86 version, or an indirect-threaded (ITC)
version of muFORTH. Type

  make native=yes

for the native x86 version, or

  make

for the indirect-threaded one.

Enjoy muFORTH!

EOF