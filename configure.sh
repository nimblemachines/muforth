#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2013 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# This not a GNU configure script!

# It configures the makefile for muforth based on which make you have,
# decides how to tell sed to use extended REs, figures out whether you're
# running Linux or OSX and configures modules accordingly, and on trees
# based on Git checkouts, sets up checkout and commit hooks to keep
# muforth's idea the "current" git commit actually current.

# If using git, configure post-commit and post-checkout scripts to generate
# a muforth file that defines the current git commit. If not using git,
# create a "fake" git-commit that pushes 0. We can check, in banner, which
# we have.

if [ -d .git ]; then
    echo
    echo "Installing post-commit, post-checkout, and post-merge hooks."
    install -m 755 post-commit.sh .git/hooks/post-commit
    install -m 755 post-commit.sh .git/hooks/post-checkout
    install -m 755 post-commit.sh .git/hooks/post-merge
    sh post-commit.sh
else
    echo ": muforth-commit 0 ;" > commit.mu4
fi

# Parse command line args
while [ "$1" ]; do
  case "$1" in
     gnu)     gnu=yes ;;
     bsd)     bsd=yes ;;
       *) ;;
  esac
  shift
done

# Set cflags and ldflags based on system. Useful for 64-bit Linux, BSD, and
# Darwin (OSX).

os=$(uname -s)
cpu=$(uname -m)

# On 32-bit hosts, silence annoying warnings about _narrowing_ casts (from
# val (64-bit) to cell (32-bit)), but leave turned on warnings about
# _widening_ casts (cell to val) so we know where they happen, and can
# specify them as sign-extending (seemingly gcc's default) or not.

Wnarrowing="-Wno-int-to-pointer-cast "
cflags=""
ldflags=""

# On 64-bit hosts, compile a native 64-bit muforth, with everything 64-bits
# wide. I'd like for muforth to feel exactly the same on all machines,
# which it now won't, but how to fix this isn't clear.

# Keep Wnarrowing, because we might be building a 32-bit executable.
# But default to whatever Darwin wants to build.
if [ "$os" = "Darwin" ]; then
    archobjs="usb-darwin.o"
    cflags="-mdynamic-no-pic"
    ldflags="-framework CoreFoundation -framework IOKit"
fi
if [ "$os" = "Linux" ]; then
    archobjs="usb-linux.o"
    if [ "$cpu" = "x86_64" ]; then
        Wnarrowing=""
    fi
    # Try to guess a device to use for serial targets
    # XXX Do this for *BSD too?
    if [ ! -c serial-target ]; then
        for term in USB0 ACM0 S0; do
            dev=/dev/tty$term
            if [ -c $dev ]; then
                ln -s $dev serial-target
            fi
        done
    fi
fi
if [ "$os" = "FreeBSD" -o "$os" = "NetBSD" ]; then
    archobjs="usb-netbsd.o usb-freebsd.o"
    if [ "$cpu" = "amd64" ]; then
        Wnarrowing=""
    fi
fi
cflags="${Wnarrowing}${cflags}"

# Figure out which version of sed we're running, so we can properly specify
# the use of extended (ie, sane) regular expressions.

echo

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

# Now, put all our variables into an "architecture-specific" make file.
cat <<EOT > arch.mk
SEDEXT=     ${sedext}
ARCH_C=     ${cflags}
ARCH_LD=    ${ldflags}
ARCHOBJS=   ${archobjs}
EOT

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
    -f scripts/make.sed \
    -e 's/^### Makefile/### BSD Makefile/' Makefile.in > Makefile
fi

cat <<EOF
Once it builds successfully, run it like this:

  ./muforth

Enjoy muFORTH!

EOF
