# This file is part of muforth: https://muforth.dev/
#
# Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

#
# It looks a bit complicated. ;-) The idea is to automagically convert from a
# list of the C names of functions that implement muforth words to a list of
# their muforth names, formatted to initialize a C array. There are a few
# exceptional cases, but mostly it's pretty straightforward.
#
# sed rocks!
#

# lose lines *not* starting with "void mu_"
/^void mu_/!d

s/^void mu_//
s/(.*)\(void\);/\1/

# don't do do_colon, do_does - they are not real forth words
#/do_/d

# now we've got the name, save it in hold space
h

s/push_//
s/_carefully//
s/less/</
s/equal/=/
#s/reset/!/
s/star(_|$)/*/
s/backslash/\\\\/
s/slash/\//
s/plus/+/
s/minus/-/
s/shift_left/<</
s/shift_right/>>/
s/fetch/@/
s/^at_/@_/
s/^size_/#_/
s/store/!/
#s/(.*)_chain/\.\1\./
s/set_(.*)_code/<\1>/
s/lbracket/[/
s/rbracket/]/
s/semicolon/;/
s/colon/:/
s/comma$/,/
s/tick/'/
s/exit/^/
s/^q/?/
s/q$/?/
s/forward/>/
s/to(_|$)/>/
s/from(_|$)/>/
s/(.*)_size/#\1/

# turn foo_ to (foo)
s/((.*)_)$/\(\2\)/

s/([a-zA-Z0-9])_([a-zA-Z0-9])/\1-\2/g
s/_//g

# concat hold & pattern
H
g

# output final string for array initializer
s/(.*)\n(.*)/	{ "\2", mu_\1 },/

