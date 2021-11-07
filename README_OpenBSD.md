# Running muforth on OpenBSD

OpenBSD, starting about 6.9ish, no longer allows applications to access /dev/ugenX.XX directly.
The OBSD devs would like you to write your own drivers.  Don't hold your breath for tools like muforth.

This creates a bit of a problem whilst attempting to chat with target devices, but huzzah, there is a
thoroughly not OBSD dev approved workaround: 

1) Put yourself on the doas list and make sure you're a member of group wheel.
2) Note which /dev/ugenX.XX enumerates the target

```
doas chmod 660 /dev/ugenX.*

or

doas chmod 660 /dev/ugenX.XX
```

accordingly, depending on how motivated you are.  Don't forget to chmod back to 600 when you're finished futzing about.
This workaround is lame and stupid and is apt to give the OBSD devs fits in the unlikely event they take note, but it gets 
the job done.  If you have oodles of time and would like to bang together platform specific drivers for muforth to run on OpenBSD, 
be our guest, be our guest, put your morals to the test.

# Supported versions

All OpenBSD testing and development on both amd64 and i386 is conducted using OpenBSD --current (7.0 as of this writing).

Older versions will likely work, as testing on OpenBSD has been ongoing since at least 5.8, but running --current is recommended.
