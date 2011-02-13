#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# This not a GNU configure script! It simply configures the makefile for
# muforth based on which make you have, and which version you want to
# build.

while [ "$1" ]; do
  case "$1" in
     gnu)     gnu=yes ;;
     bsd)     bsd=yes ;;
       *) ;;
  esac
  shift
done

# Set archcflags and archldflags based on system. Useful for 64-bit Linux
# and for Darwin (OSX).

os=$(uname -s)
cpu=$(uname -m)

# On 32-bit hosts, silence annoying warnings about _narrowing_ casts (from
# cell (64-bit) to address (32-bit)), but leave turned on warnings about
# _widening_ casts (address to cell) so we know where they happen, and can
# specify them as sign-extending (seemingly gcc's default) or not.

archcflags="-Wno-int-to-pointer-cast"
archldflags=""

if [ "$os" = "Darwin" ]; then
    archcflags="-m64"
    archldflags="-m64"
fi
if [ "$os" = "Linux" -a "$cpu" = "x86_64" ]; then
    archcflags="-m64"
    archldflags="-m64"
fi

# Figure out which version of sed we're running, so we can properly specify
# the use of extended (ie, sane) regular expressions.

if sed --version 2> /dev/null | grep -q "GNU"; then
  cat <<EOF
Found GNU sed; using "-r" for extended regular expressions.
EOF
  sedext="-r"
else
  cat <<EOF
Found BSD sed; using "-E" for extended regular expressions.
EOF
  sedext="-E"
fi

# fix up use of sed in scripts/do_sed.sh
sed ${sedext} \
  -e "s/%sedext%/${sedext}/g" \
  scripts/do_sed.sh.in > scripts/do_sed.sh
chmod 755 scripts/do_sed.sh
 
# Figure out which version of make we're using (most likely GNU or BSD) and
# set up an appropriate Makefile.

if [ "${gnu}" = "yes" ] || 
   ([ "${bsd}" != "yes" ] &&
    make --version 2> /dev/null | grep -q "GNU Make"); then
  cat <<EOF
Found GNU make; creating a GNU-compatible Makefile.
If instead you want to force the creation of a BSD-compatible Makefile,
re-run configure.sh like this:

  ./configure.sh bsd

Then run your BSD make.

EOF
  sed ${sedext} \
    -e "s/%sedext%/${sedext}/g" \
    -e "s/%archcflags%/${archcflags}/g" \
    -e "s/%archldflags%/${archldflags}/g" \
    -f scripts/make.sed \
    -f scripts/gnu-make.sed \
    -e 's/^### Makefile/### GNU Makefile/' Makefile.in > Makefile
else
  cat <<EOF
Found non-GNU (perhaps BSD?) make; creating a BSD-compatible Makefile.
If the build fails, try re-running configure like this:

  ./configure.sh gnu

Then type "gmake" instead of "make".

EOF
  sed ${sedext} \
    -e "s/%sedext%/${sedext}/g" \
    -e "s/%archcflags%/${archcflags}/g" \
    -e "s/%archldflags%/${archldflags}/g" \
    -f scripts/make.sed \
    -e 's/^### Makefile/### BSD Makefile/' Makefile.in > Makefile
fi

cat <<EOF
Once it builds successfully, run it like this:

  ./muforth

Enjoy muFORTH!

EOF
