#################################################
Fri Nov 19 16:31:13 PST 2021
OpenBSD 7.0
GENERIC.MP#298 i386
#################################################

Really good news:

Recent support has been added to address issue #31, and as a side effect
help somewhat with issue #32.

Let's see if they work:

---------------------------------------------------------------------

STM32F407G-Discovery board:
Loaded code is MISMATCHED with flashed target:

jtag 
ST-LINK/V2 JTAG v16 
DFU mode => Mass mode => Debug mode 
      SP        RP        IP
2001fed8  00000000  00000000* 
WARNING: Target's flash image differs from host's. Run flash-image? Ok
(chatting) (hex) (flash)

This is an obvious and mostly reasonable message, however the inclusion
of a question mark followed by the canonical Ok response is apt to lead
the new or inexperienced user to conclude that the board was just
automagically flashed.  It might, perhaps, be better to phrase as a
statement: 'You can either run flash-image followed by verify, or you may
CTRL-C out of here, replug, and retry by loading the file(s) that will 
match the flashed target.'  A bit prolix perhaps, but it leaves no room
for doubt as to what the hell is going on and what a possible solution
may be.

----------------------------------------------------------------------

STM32F407G-Discovery board:
Loaded code is MATCHED with flashed target:

jtag 
ST-LINK/V2 JTAG v16 
DFU mode => Mass mode => Debug mode 
      SP        RP        IP
2001fed8  00000000  00000000*  Ok (chatting) (hex) (flash)

Well there's no message one way or the other is there?  Should we assume
everything is just peachy? Should we go ahead and manually verify? Was
this load handled obviously?

We have to conclude that no, this is not obvious, and if we have a
message delivered for one condition but not another then we're left in a
state of lingering doubt.  Let's go ahead and type verify just to be
sure.  Regretfully, we must conclude that this is marked as a failure of
the intent to provide verification of the flashed image.  If the damn
thing is verified, state that: "Flash image and host image match and are
verified"  or  "Verification successful; flash image and host image
match"

verify  Ok (chatting) (hex) (flash)

Now this is truly fascinating, because the problem of assumption that
arises due to the canonical Forth prompt is **reversed** from that
above:  the new or inexperienced user now assumes that the verification
is Ok because that's what the program replied, when in fact the Ok just
means "Hey sailor, Ok, peachy, we're still chatting and whatever you did just
successfully executed." It most definitely does NOT mean that the
verification is Ok.  The **lack of additional output** returned by
running verify means everything is ok ... but how the hell is the new or
unfamiliar user supposed to know that unless they've read the
non-existent documentation?

-----------------------------------------------------------------------

Okey dokey, let's actually peek into the code and see what was added in
terms of communicating verification to the user.

```
git show 9fe16ee57c358a8dff0c8d7ab0e6b4355baae0e9
```

Looks like we have new messages for the following words:

```
?align-region
WARNING: The flash region does not end on a target cell boundary

?empty-region
WARNING: The flash region is empty. verify will always report no change.

hi
WARNING: Target's flash image differs from host's. Run flash-image?
```

That's it! If you think something else needs to be added, file on issue
on github and we'll take a look at it.
