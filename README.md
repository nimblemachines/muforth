# muforth

Fork of https://github.com/nimblemachines/muforth

New users might want to watch or clone this repository, as it actively tries
to identify and offer solutions for tripping points or omissions that
have not yet been incorporated into the master repository above.

A case in point until **very** recently:

If you are a new user with a fresh factory stm32fx-discovery board and
you attempt to chat with the target using the appropriate default
target/ARM/board/ file **in the master repository**, you're going to have
some weird moments with flash because those files now load "special" example
code.  You can either reflash the board with that code OR you can just
use the target/ARM/board files **in this repository** clearly labelled with 
the word "factory".

Fortunately, some new verification code was just pushed, and it now warns the
user if the there is a mismatch between target/host flash. Also see [this writeup](https://github.com/anarchitech/muforth/blob/master/mu/target/ARM/beta-testing/testing-flash-verification-code.md)
for some further thoughts regarding this and ui in general.

Documentation for muforth is, at present, scattered between git logs and
code comments; improving this is at the top of everyone's list. 


## Supported platforms/architectures

Currently tested and working on the following systems/platforms:

* OpenBSD (i386, amd64)
* Linux (Archlinux, Gentoo, Debian, Alpine) (x86_64, aarch64)
* NixOS (x86_64)
* Termux (aarch64)
* Cros (via chroot or that ... crostini ... thing.)


## Building

For Linux any flavor, NixOS and Termux, use the master branch.  For the BSD's use
the bsd-usb-support branch.  Ask daf about OSX/iOS and Windows support, I
have no idea where we are with any of those. (I doubt there's any
support for iOS, although you never know. Cygwin used to work with
Windows. I **once** got it compile on Windows. **Once**. Don't ask,
the therapy bill was expensive.  

cd into the cloned repository, run ./configure.sh and then run make.
Note the admonition about 99-muforth.rules

## Running

You can use muforth as a standalone forth, and this is a lot of fun for
learning about muforth, and a bit about forth.  Be forewarned that if
you use one of the classics like Leo Brodie's Starting Forth you are
going to discover that like all true forth's, muforth is its own animal.
What's the rubric? "If you've learned one Forth, you've learned one
Forth."

The real fun with muforth comes with connecting to **targets** -- either a
lone processor with some supporting hardware wired up to it or one of
the nifty low cost development boards out there. Sadly, the era of DIP's
and breadboarding seems to be waning; it was a really "hands on" way to
roll up a cheap experiment.  I'm still miffed about the LPC810.

Chatting with a target depends on whether it uses serial or jtag and to
a certain extent it depends on which OS.  OpenBSD, for example, uses
/dev/cuaXX to enumerate serial devices. Linux uses /dev/ttySx ... most
of the time.  You'll have to cd into muforth/mu, then ln -s /dev/whateva
serial-target.

For jtag targets on Linux it tends to "just work" out of the box without
worrying too much about /dev/whatamienumeratingas

On OpenBSD, due to the dim view the devs have of the security of the USB
code, access is now restricted, but you can get it by using doas and
making yourself a member of group wheel. doas chmod 660 /dev/ugen0.*
generally is the fastest way to get jtag working with muforth targets.

Generally speaking, jtag or serial, you're going to cd into muforth/mu.
Using the Raspberry Pi Pico as an example:

``` 
./muforth -f target/ARM/board/raspberry-pi-pico.mu4 jtag 
```

and you're off to thrashy bit land.  @ram puts you in ram, @flash puts
you in flash. du dumps memory from address, dis disassembles code, and
dec decompiles forth code on a target.

## Jumping Jack Flash

You can use muforth to flash a target with your very own code once you
get up to speed!  In virtually all cases, we highly recommend saving one
or more memory images of a fresh target ... grab anything you can and
save it, but especially grab everything in flash.  You'll thank me
later. ;)

### Example: Saving a flash image file

Load a file and start chatting with the target. Put it into hex mode.
Let's assume in the following that you have a minty fresh
stm32f407g-discovery board.

```
cd muforth/mu
./muforth -f target/ARM/board/stm32f4-factory-discovery.mu4
jtag
@flash du 
```
   ... and keep hitting return until you find nothing but ff's

 Note the address where you start hitting ff's; in fact, pick the next
 line that is all ff's, the padding won't hurt a thing.  In the case
 of the 407, @flash started at 08000000 and ended about 08004590.


```
drop
```  
drop gets rid of anything left on the stack

```
 @flash 4590 read-image
```
This puts the address of flash, followed by the size, onto the stack and
then executes read-image, which reads the image into ram.

```
save-image fileName.img
```  
This will automagically save fileName.img in muforth/mu/ and *no*,
you do not need to enclose the fileName in quotes, backticks or any 
such nonsense. 
 
### Example: Load procedure for a saved or new .img file

You need to chat with the board and load the image file you intend to
flash to the target:

```
cd muforth/mu
./muforth -d testing -f target/ARM/board/stm32f4-discovery.mu4 load-image stm32f407.img

jtag

flash-image

verify
```

If it all worked, verify will just print "Ok". If it didn't, verify
will spew a range of exciting addresses at you and then print "Ok." That
"Ok" does not mean everything is "OK" it means "Ok, verify executed successfully"
but that nasty spew means the target host flash verification failed.
Sometimes "Ok" is not "Ok", children, Mmmmm-kay? (I'm also not a fan of Forth's
Ok prompt. I'm in the minority.)

## Going Forward

Your best path to understanding what to do after that is going to come
from notes in the repo, git logs and code.  We're working on making the
documentation more organized, extensive and complete, but it's going to
take some time. 

Reach out to the devs, they respond to emails. Pull requests seem to
sort of stagnate, and obviously until of late issues on github haven't
exactly been a thing, but hopefully that's changing.  There is no slack,
discord or libera channel (yet).  There's a not terribly active twitter
account, @muforth.  Twitter is a dumpster fire and most of us would
prefer to spend our time banging away here than hanging out in that
silly place.

Finally, a word of encouragement.  I freely admit to a love/hate
relationship with Forth. I really love it when I'm actively using it. I
utterly loathe it when I'm trying to read somebody else's code. Don't
despair, document. Document the code you write, document the code of
others you read. Use paper and pencil, wood blocks with tape and magic
marker, lego bricks with same or a whiteboard and work out what is
happening on the stack with the code. 

Forth, more than any other language, really benefits from a literate
programming style. BE OBVIOUS in the code you write. **Don't be clever.**
Ok, maybe be clever, because that's fun, but DOCUMENT your cleverness.
And by document, I absolutely DO NOT mean simply relying on traditional 
Forth stack comments.  I HATE Forth's stack comments, they really annoy
me. It's a baseless, pathological hatred. Fortunately, muforth provides
several ways to comment code!

```
( This is a comment on one line )

-- This comments out this line

comment SQUIDATTACK

This is a block of comment text that will remain a block of comment text
until I type the comment token SQUIDATTACK on a line by itself. This
comment form is very handy for writing a literate programming file.

Speaking of which: one technique that works well is to have a highly
commented, literate programming file for your records or a client, and
then a separate target code file. Both contain the same executable code,
the latter is just much more terse and compact. It can really help when
you revist something days, months or years later to have that literate
file as a reference document. 

SQUIDATTACK
```

***PLAY WITH THE STACK.***

```
rot, swap, dup, over, drop, <<, >>, @, !, c@, c! ...
``` 

The stack is where it all happens.  The Pico with its 8 stack machines
has us in absolute drool mode.  Good things will be happening there.

muforth is an excellent tool for fast testing and prototyping of
supported targets.  More targets will be supported in the future, and if
you have a target you think is neat that you'd like to try muforth on,
reach out.  Right now we're really focusing on ARM. muforth's goal isn't 
necessarily to provide a complete development environment ... we're not
into giving you free fish, we're interested in teaching you how to fish
and giving you the tools to do so. 
