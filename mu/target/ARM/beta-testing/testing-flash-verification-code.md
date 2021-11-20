Really good news:

Recent support for target/host flash verification has been added that helped us decide 
to close issue #31 and #32, not without minor disagreement covered below. (There are merits
to both sides of this.)

Let's see how it works:

---------------------------------------------------------------------

STM32F407G-Discovery board:
Loaded code is MISMATCHED with flashed target:
```
jtag 
ST-LINK/V2 JTAG v16 
DFU mode => Mass mode => Debug mode 
      SP        RP        IP
2001fed8  00000000  00000000* 
WARNING: Target's flash image differs from host's. Run flash-image? Ok
(chatting) (hex) (flash)
```
This is an obvious and mostly reasonable message, however the inclusion
of a question mark followed by the canonical Ok response is apt to lead
the new or inexperienced user to conclude that the board was just
automagically flashed.  It might, perhaps, be better to phrase as a
statement: 'Run flash-image or reload with the correct files.'  A bit prolix perhaps, 
but it leaves no room for doubt as to what the hell is going on and what a possible 
solution may be. It also reduces the potential ambiguity of ? Ok

----------------------------------------------------------------------

STM32F407G-Discovery board:
Loaded code is MATCHED with flashed target:
```
jtag 
ST-LINK/V2 JTAG v16 
DFU mode => Mass mode => Debug mode 
      SP        RP        IP
2001fed8  00000000  00000000*  Ok (chatting) (hex) (flash)
```
Well there's no message one way or the other is there?  Should we assume
everything is just peachy? Should we go ahead and manually verify? Was
this load handled obviously?

We have to conclude that no, this is not obvious, and if we have a
message delivered for one test of a condition but not another then we're left 
in a state of lingering doubt.  

Let's go ahead and type verify just to be sure:
```
verify  Ok (chatting) (hex) (flash)
```
Now this is truly fascinating, because the problem of assumption that
arises due to the canonical Forth prompt is **reversed** from that
above:  the new or inexperienced user might assume that the verification
is Ok because that's what the interpreter replied, when in fact the Ok just
means "Hey sailor, Ok, peachy, we're still chatting and the word verify just
successfully executed." It most definitely does NOT state that the actual
verification is Ok.  Unbeknownst to the new user, it is the **lack of additional output** 
returned by running verify that means verification is ok ... but how the hell is 
the new or inexperienced user supposed to know that unless they've read the
non-existent documentation? (It must be admitted that I have always had an
issue with Forth's use of Ok; as shown above it is an imprecise choice for a humane
interface and Forth would be better served with a simple prompt for its interpreter.)

Regretfully, we must conclude that this must be marked as a failure of
the intent to provide verification of the flashed image.  If the damn
thing is verified, state that: "Flash image and host image match and are
verified"  or  "Verification successful; flash image and host image
match"

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

That's it! If you think something else needs to be added, file an issue
on github and we'll take a look at it.
