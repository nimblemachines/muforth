#!/bin/sh

# $Id$

# configures which make to use
if make --version 2> /dev/null | grep -q "GNU Make"; then
cat <<EOF
Looks like you're running GNU make. I'm going to create a compatible
makefile for you.
EOF
  sed -E -f make.sed Makefile.in > Makefile
  echo "Done."
else
  cat <<EOF
Looks like you're running a non-GNU (perhaps BSD?) make. I'm going to
assume it's a BSD make and create a makefile for you. If the build
fails, try running "gmake" instead of "make".
EOF
  cp Makefile.in Makefile
  echo "Done."
fi

cat <<EOF
You can build a native x86 version, or an indirect-threaded (ITC) version
of muFORTH.

Type

  make X86=yes

for the native version, or

  make ITC=yes

for the indirect-threaded one.

Enjoy muFORTH!
EOF