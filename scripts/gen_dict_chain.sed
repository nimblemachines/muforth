# $Id$
#
# This file is part of muforth.
#
# Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
# reversed.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# or see the file LICENSE in the top directory of this distribution.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# It looks a bit complicated. ;-) The idea is to automagically convert from
# a list of the C names of functions that implement muforth words to a list
# #of their muforth names, formatted to initialize a C array. There are a
# few exceptional cases, but mostly it's pretty straightforward.
#
# sed rocks!
#

# lose lines *not* starting with "void mu_"
/^void mu_/!d

s/^void mu_//
s/(.*)\(void\);/\1/

# don't do do_colon, do_does - they are not real forth words
/do_/d

# now we've got the name, save it in hold space
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
s/from/>/
s/(.*)_size/#\1/

# turn foo_ to (foo)
s/((.*)_)$/\(\2\)/

s/([a-z])_([^a-z])/\1\2/g
s/([^a-z])_([a-z])/\1\2/g
s/([^a-z])_([^a-z])/\1\2/g
s/([a-z])_([a-z])/\1-\2/g

# concat hold & pattern
H
g

# output final string for array initializer
s/(.*)\n(.*)/	{ "\2", mu_\1 },/




