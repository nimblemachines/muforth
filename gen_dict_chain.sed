#
# $Id$
#
# It looks a bit complicated. ;-) The idea is to automagically convert from
# the C name of a function (that implements a muforth word) to its muforth
# name. There are a few exceptional cases, but mostly it's pretty
# straightforward.
#
# sed rocks!
#

# lose lines *not* starting with "void mu_"
/^void mu_/!d

s/^void mu_//
s/(.*)\(void\);/\1/

# don't do do_colon, do_does - they are not real forth words
/do_/d

# now we've got the name
h

s/push_//
s/_carefully//
s/less/</
s/equal/=/
s/zero/0/
s/reset/!/
s/star(_|$)/*/
s/slash/\//
s/plus/+/
s/minus/-/
s/shift_left/<</
s/shift_right/>>/
s/fetch/@/
s/store/!/
s/(.*)_chain/\.\1\./
s/set_(.*)_code/<\1>/
s/lbracket/[/
s/rbracket/]/
s/semicolon/;/
s/colon/:/
s/comma$/,/
s/tick/'/
s/exit/^/
s/q/?/
s/(.*)_size/#\1/

# turn foo_ to (foo)
s/((.*)_)$/\(\2\)/

s/([a-z])_([^a-z])/\1\2/g
s/([^a-z])_([a-z])/\1\2/g
s/([^a-z])_([^a-z])/\1\2/g
s/([a-z])_([a-z])/\1-\2/g

H
g
s/(.*)\n(.*)/	{ "\2", mu_\1 },/




