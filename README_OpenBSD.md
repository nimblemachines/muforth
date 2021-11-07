# Running muforth on OpenBSD

OpenBSD, starting about 6.9ish, no longer allows applications to access /dev/ugenX.XX directly.
The OBSD devs would like you to write your own drivers.  Don't hold your breath for tools like muforth.

This creates a bit of problem whilst attempting to chat with target devices, but huzzah, there is a
thoroughly not OBSD dev approved workaround: 

1) Put yourself on the doas list and make sure you're a member of group wheel.
2) Note which /dev/ugenX.XX enumerates the target

```
doas chmod 660 /dev/ugenX.*

or

doas chmod 660 /dev/ugenX.XX
```

accordingly, depending on how motivated you are.  This is suggestion is lame and stupid and is apt to
give the OBSD dev's fits in the unlikely event they take note, but it gets the job done.  If you have oodles
of time and would like to bang together platform specific drivers for muforth to run on OpenBSD, be our guest,
be our guest, put your morals to the test.
