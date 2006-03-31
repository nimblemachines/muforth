#!/bin/sh

# $Id$

# This not a GNU configure script! It simply configures the makefile for
# muforth based on which make you have, and which version you want to
# build.

# default to ITC
engine=itc

while [ "$1" ]; do
  case "$1" in
    i386)  engine=i386 ;;
     gnu)     gnu=yes ;;
       *) ;;
  esac
  shift
done

if [ "${gnu}" = "yes" ] || 
    make --version 2> /dev/null | grep -q "GNU Make"; then
  cat <<EOF
Looks like you're running GNU make. I'm going to create a compatible
makefile for you.

EOF
  sed -E \
    -e s/#engine#/${engine}/ \
    -f scripts/make.sed \
    -f scripts/gnu-make.sed \
    -e 's/^### Makefile/### GNU Makefile/' Makefile.in > Makefile
else
  cat <<EOF
Looks like you're running a non-GNU (perhaps BSD?) make. I'm going to
assume it's a BSD make and create a makefile for you. If the build
fails, try re-running configure like this:

  ./configure [i386] gnu

Then type "gmake" instead of "make".

EOF
  sed -E \
    -e s/#engine#/${engine}/ \
    -f scripts/make.sed \
    -e 's/^### Makefile/### BSD Makefile/' Makefile.in > Makefile
fi

cat <<EOF
There are two versions of muFORTH: you can build a native x86 version, or
an indirect-threaded (ITC) version. This script defaults to configuring
the ITC version; to build the native x86 version, re-run the script like
this:

  ./configure.sh i386

To build it, just type

  make

Enjoy muFORTH!

EOF
