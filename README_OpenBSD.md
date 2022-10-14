# Running muforth on OpenBSD

### UPDATE: OpenBSD now uses master branch! 

NetBSD, FreeBSD and with any luck DragonFlyBSD may be able to use the
bsd-usb-support branch.  Theoretically this will all get folded into
master at some point.  For now, if you *do* use one of these systems,
you'll find you frequently have to ```git merge origin master``` as
syncing that branch is quite often delayed. 

## JTAG Supported Targets

OpenBSD, starting about 6.9ish, no longer allows applications to access /dev/ugenX.XX directly.
The OBSD devs would like you to write your own drivers.  Don't hold your breath for tools like muforth.

This creates a bit of a problem whilst attempting to chat with target devices that connect via jtag to /dev/ugenX.XX, but huzzah, there is a
thoroughly not OBSD dev approved workaround: 

1) Put yourself on the doas list and make sure you're a member of group wheel.
2) Note which /dev/ugenX.XX and which /dev/usbX enumerate the target

```
doas chown $USER /dev/ugenX.* /dev/usbX

```

This workaround is lame and stupid and is apt to give the OBSD devs fits in the unlikely event they take note, but it gets 
the job done.  If you have oodles of time and would like to bang together platform specific drivers for muforth to run on OpenBSD, 
be our guest, be our guest, put your morals to the test.

## Serial Targets

You have to be a member of group **dialer**!

Devices such as the HCS08 and the MSP430 where the host uses a PL2303 or FTDI2232H to connect to the target:

```
cd muforth/mu
ln -s /dev/cuaU0 serial-target
```

Devices such as the SiFive's RISC-V HiFive1 that connect via serial chat as opposed to jtag using a direct connection with a USB cable:

```
cd muforth/mu
ln -s /dev/cuaU1 serial-target
```

# Supported OpenBSD versions

All OpenBSD testing and development of muforth on both amd64 and i386 is conducted using OpenBSD --current (7.2 as of this writing).

Older versions will likely work, as testing on OpenBSD has been ongoing since at least 5.8, but running --current is recommended.
