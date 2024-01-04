#!/bin/sh

# This file is part of muforth: https://muforth.dev/
#
# Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

# This NOT a GNU configure script!

# It configures the makefile for muforth based on which make you have,
# decides how to tell sed to use extended REs, figures out whether you're
# running Linux or OSX and configures modules accordingly, and on trees
# based on Git checkouts, sets up checkout and commit hooks to keep
# muforth's idea the "current" git commit actually current.

# If using git, configure post-commit and post-checkout scripts to generate
# a muforth file that defines the current git commit. If not using git,
# create a "fake" git-commit that pushes 0. We can check, in banner, which
# we have.

# Useful for scripts, so they know where they are
top=$(dirname $(pwd))

# Where to put temporary files
# Use TMPDIR if defined and non-empty, otherwise /tmp
tmp=${TMPDIR:-/tmp}

if [ -d ../.git ]; then
    # Create the hook script for Git
    cat <<EOT > ${tmp}/post-commit.sh
#!/bin/sh

# This file is part of muforth: https://muforth.dev/
#
# Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

# So that we always have an accurate git commit available to Forth code,
# after commit, checkout or merge generate a muforth file that defines the
# current commit.

echo "Generating commit.mu4"

sha=\$(git rev-parse --verify HEAD)

echo ": muforth-commit  \" \$sha\" ;" > ${top}/mu/commit.mu4
EOT
    echo
    echo "Installing post-commit, post-checkout, and post-merge hooks."
    install -m 755 ${tmp}/post-commit.sh ../.git/hooks/post-commit
    install -m 755 ${tmp}/post-commit.sh ../.git/hooks/post-checkout
    install -m 755 ${tmp}/post-commit.sh ../.git/hooks/post-merge
    sh ${tmp}/post-commit.sh
    rm ${tmp}/post-commit.sh
else
    echo ": muforth-commit 0 ;" > ../mu/commit.mu4
fi

# Parse command line args
while [ "$1" ]; do
  case "$1" in
     gnu)     gnu=yes ;;
     bsd)     bsd=yes ;;
     force32)   force32=yes ;;
     force64)   force64=yes ;;
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
# wide. On 32-bit hosts, compile an "emulated" 64-bit muforth - with cells
# and stacks 64 bits wide, and C pointers "wrapped" in 64-bit cells.

# Keep Wnarrowing, because we might be building a 32-bit executable.
# But default to whatever Darwin wants to build.
if [ "$os" = "Darwin" ]; then
    archobjs="file.o main.o time.o tty.o select.o pty.o usb-darwin.o"
    cflags="-mdynamic-no-pic"
    ldflags="-framework CoreFoundation -framework IOKit"
fi
if [ "$os" = "Linux" ]; then
    archobjs="file.o main.o time.o tty.o select.o pty.o usb-linux.o"

    if [ "$cpu" = "x86_64" ]; then
        Wnarrowing=""
    fi
    # Try to guess a device to use for serial targets
    # XXX Do this for OSX and *BSD too? And match all USB known USB serial
    # devices?
    if [ ! -c ../mu/serial-target ]; then
        for term in USB0 ACM0 S0; do
            dev=/dev/tty$term
            if [ -c $dev ]; then
                ln -s $dev ../mu/serial-target
            fi
        done
    fi
    if [ -d /dev/bus/usb ]; then
        cat <<EOT

Your USB devices are mounted on /dev/bus/usb and controlled by udevd.

EOT
        if [ -f /etc/nixos/configuration.nix ]; then
            # Running NixOS! Yippee!
            ../scripts/make-nix-config.sh $USER > udev.nix
            cat <<EOT
It appears that you are one of the few intrepid souls running NixOS.

We salute you!

If you want to use USB devices with muforth, you'll want to take the
udev.nix file that was just generated, which contains a service definition
and a few udev rules, and append it to your configuration.nix.

After doing a 'nixos-rebuild switch' or 'nixos-rebuild test' the new rules
should be available to udev.
EOT
        else
            ../scripts/make-udev-rules.sh $USER > 99-muforth.rules
            ../scripts/make-udev-rules.sh $UID > 99-muforth-uid.rules
            cat <<EOT
A udev rules file has just been generated, which will be of interest to you
if you want to use USB devices with muforth.

Please do the following as root:

  # cp 99-muforth.rules /etc/udev/rules.d/
  # udevadm control --reload

(or possibly some other command to tell udevd to reload its rules files;
'man udev' for the whole story).

If you are running this on a Chromebook, things are slightly more
complicated. /etc is on a read-only filesystem. The udev rules live in a
volatile filesystem (/run) and have to be re-copied every time you restart
your Chromebook, so you'll want to make a script. But the basic steps are:

In a non-chroot shell,

  $ sudo mkdir -p /run/udev/rules.d
  $ sudo cp ${muforth}/src/99-muforth-uid.rules /run/udev/rules.d
  $ sudo udevadm control --reload

Note that this way uses a different rules file; this one has a numeric UID
rather than a user name for each rule, since the Chromebook's /etc/passwd
and /etc/group files aren't going to have the user name info that lives in
your chroot.

EOT
        fi
    elif [ -d /proc/bus/usb ]; then
        cat <<EOT

Your USB devices are mounted on /proc/bus/usb and controlled by usbfs.
If you want to use USB devices with muforth you'll want to modify the
/etc/fstab line that mounts the usbfs to allow regular users to access
USB device files.
EOT
    fi
fi

if [ "$os" = "FreeBSD" ]; then
    # For FreeBSD, include both old-style and new-style USB drivers. Let
    # the C preprocessor decide which code to include. ;-)
    archobjs="file.o main.o time.o tty.o select.o pty.o usb-netbsd.o usb-freebsd.o"
    if [ "$cpu" = "amd64" ]; then
        Wnarrowing=""
    fi
fi

if [ "$os" = "DragonFly" -o "$os" = "NetBSD" -o "$os" = "OpenBSD" ]; then
    # DragonFly, NetBSD, and OpenBSD all have a NetBSD-like USB stack.
    archobjs="file.o main.o time.o tty.o select.o pty.o usb-netbsd.o"
    if [ "$cpu" = "amd64" -o "$cpu" = "x86_64" ]; then
        Wnarrowing=""
    fi
fi

if [ "$force32" = "yes" ]; then
    cflags="-m32 -Wno-int-to-pointer-cast ${cflags}"
    ldflags="-m32 ${ldflags}"
elif [ "$force64" = "yes" ]; then
    cflags="-m64 ${cflags}"
    ldflags="-m64 ${ldflags}"
else
    cflags="${Wnarrowing}${cflags}"
fi

# Now, put all our variables into an "architecture-specific" make file.
cat <<EOT > arch.mk
ARCH_C=     ${cflags}
ARCH_LD=    ${ldflags}
ARCHOBJS=   ${archobjs}
MU_DIR=     ${top}/mu
EOT

# Touch local.mk, which we'll need regardless of which kind of Make the
# user has. Also, this allows us to use .include rather than .sinclude,
# which doesn't exist in OpenBSD.
touch local.mk

# Figure out which version of make we're using (most likely GNU or BSD) and
# set up an appropriate Makefile.

if [ "${gnu}" = "yes" ] ||
   ([ "${bsd}" != "yes" ] &&
    make --version 2> /dev/null | grep -q "GNU Make"); then
  cat <<EOT
Found GNU make; creating a GNU-compatible Makefile.
If instead you want to force the creation of a BSD-compatible Makefile,
re-run configure.sh like this:

  ./configure.sh bsd

Then run your BSD make.

EOT
  sed -E \
    -f ../scripts/make.sed \
    -f ../scripts/gnu-make.sed \
    -e 's/^### Makefile/### GNU Makefile/' Makefile.in > Makefile
else
  cat <<EOT
Found non-GNU (perhaps BSD?) make; creating a BSD-compatible Makefile.
If the build fails, try re-running configure like this:

  ./configure.sh gnu

Then type "gmake" instead of "make".

EOT
  sed -E \
    -f ../scripts/make.sed \
    -e 's/^### Makefile/### BSD Makefile/' Makefile.in > Makefile
fi

cat <<EOT
Once it builds successfully, run it like this:

  cd ../mu
  ./muforth

Enjoy muforth!

EOT
