" This file is part of muforth: https://muforth.dev/

" Copyright 2002-2024 David Frech. (Read the LICENSE for details.)

" mu4 (muforth) settings for Vim

" expand tabs into spaces
setlocal expandtab

" indent by 3
setlocal shiftwidth=3

" softtabstop makes things better!
" allows the hard tabstop to remain set to 8, but using <BS> and <Tab>
" it feels like real tab are being used.
setlocal softtabstop=3

" smarttab too!
" makes tabbing and BSing at the start of a line feel like using real tabs.
setlocal smarttab

" I've edited a *lot* of muforth code with this set to 75. Changing this might
" cause a lot of different reformatting!
setlocal textwidth=75

" In my ~/.vimrc I have the following:
"   set formatoptions=cqlor1j
" That option interacts with how comments work; read the Vim manual for the
" (complicated!) details!
"
" Set comment strings.
" Using "|" as a "whole-line" comment char creates a nice left-hand vertical
" bar, visually setting off the comments from the code.
" Using "--" as before is also possible.
" These are the only comments that Vim knows about. Use them and not
" parentheses for block comments! Everything will work much better. ;-)
setlocal comments=b:--,b:\|

" In Forth anything can be a keyword; let's let Vim know that. @ means all
" alphanumeric (including international characters); then we add all the
" printable ASCII, including punctuation.
"
" I used to exclude parens, hyphens, and underscores from iskeyword, in order
" to make it easier to edit compound names, and also easier to *not*
" accidentally edit (and generally elide) the closing right paren of
" a comment. However, I want *completion* to work well - including completion
" in included files! For this to work, iskeyword needs to be correctly
" defined. In particular, it needs to include dashes/hyphens, underscores, and
" parens.
setlocal iskeyword=@,!-~

" Set the path for completions. Sadly this will only work in directories
" that are called "muforth", not "muforth-clean" or whatever. I sometimes
" have several checkouts of muforth side-by-side.
" XXX Is there an easy way to do this better? What we really want to do is to
" cd up to the nearest enclosing mu/ directory and use that. How?
setlocal path=$HOME/muforth/mu

" Find included files. Look for a line starting with "ld" or "ld!".
setlocal include=^\\s*ld!\\?
