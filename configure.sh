#!/bin/sh

# $Id$

# This not a GNU configure script! It simply configures the makefile for
# muforth based on which make you have, and which version you want to
# build.

# default to ITC
engine=itc

while [ "$1" ]; do
  case "$1" in
     gnu)     gnu=yes ;;
       *) ;;
  esac
  shift
done

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
  
if [ "${gnu}" = "yes" ] || 
    make --version 2> /dev/null | grep -q "GNU Make"; then
  cat <<EOF
Found GNU make. I'm going to create a compatible makefile for you.

EOF
  sed ${sedext} \
    -e "s/%sedext%/${sedext}/g" \
    -e s/%engine%/${engine}/ \
    -f scripts/make.sed \
    -f scripts/gnu-make.sed \
    -e 's/^### Makefile/### GNU Makefile/' Makefile.in > Makefile
else
  cat <<EOF
Found non-GNU (perhaps BSD?) make. I'm going to assume it's a BSD make and
create a makefile for you. If the build fails, try re-running configure
like this:

  ./configure gnu

Then type "gmake" instead of "make".

EOF
  sed ${sedext} \
    -e "s/%sedext%/${sedext}/g" \
    -e s/%engine%/${engine}/ \
    -f scripts/make.sed \
    -e 's/^### Makefile/### BSD Makefile/' Makefile.in > Makefile
fi

cat <<EOF
Once it builds successfully, run it like this:

  ./muforth

Enjoy muFORTH!

EOF
