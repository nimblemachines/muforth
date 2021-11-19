# Flashing Newbies

A word of warning: if you're a newbie and you plug in your nice new
stm32f0/f4/f3-discovery board ... or several others ... and things get
strange with flash, it's because at the present time muforth's default
loader files load "special" example code.

Try it!

Plug in your board, jtag and hit verify.  A terrifying dump of weird
numbers will shoot past followed by an ok.  This is because host side is
not matching target side.

An argument has been made that this is less than ideal and we need two
loader files, one for factory images and one for "special" example code,
because faceplanting a new user like this violates the principle of
least astonishment.  So in this repo, you get both, clearly labelled.

Just look for the word "factory" in the loader file and use that with a
minty fresh board.

NB: At the present time, stm32l-discovery.mu4 and
stm32l-factory-discovery.mu4 are equivalent.

Cheers.
