# ChromeOS Terminal

For ChromeOS devices (chromebooks) running with Linux apps, the Terminal app
requires special handling while using muforth to chat with RISC-V
devices, particularly HiFive's line of SBC's.

You will need to edit target/RISC-V/chat-host.mu4 and in the definition of 
c.hello change the chunk size from #256 to #32, otherwise that chat connection
will time out and you will make sad noises.

This **only** holds true for the Terminal app! If you opt for Developer mode and run
a chroot (Archlinux is our preference) everything will work just swimmingly.

Furthermore, it should be noted that when the HiFive1 is first plugged into the 
chromebook's USB port, there will be a small pop-up that offers you to connect the
device to Linux. You **must** select this if using Terminal.  You must **ignore** this
if you are connecting via a chroot.

External USB device support has frankly been pathetic on the part of ChromeOS.  We gave
up hope recently and either pwn the chromebooks outright and change them into Linux boxen,
or opt for developer mode and a chroot (new models like the Acer Spin 713).  While we
were recently pleased to discover **somewhat working** support for external USB devices via
Terminal, it is ... twitchy to say the least, and direly needs improvement.

In general, if you experience difficulty chatting with a target, always try tweaking the
chunk size; the default is #256 and we advise just cutting that value in half until it works.
