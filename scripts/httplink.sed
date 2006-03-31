# $Id$
# Make a link to nimblemachines in all the source code.

# this matches the C comment
/^ \* This file is part of muforth/ {
s/muforth./muFORTH; for project details, visit/
a\
\ *\
\ *\ \ \ \ http:\/\/nimblemachines.com\/browse?show=MuForth
}

# this matches muforth comment
/^  This file is part of muforth/ {
s/muforth./muFORTH; for project details, visit/
a\
\
\ \ \ \ \ http:\/\/nimblemachines.com\/browse?show=MuForth
}

# put a link into the startup banner
/\( ### httplink ###/ {
# get next line
N
s/^(.*)\n(.*)/\1\
\ ."\ \ \ http:\/\/nimblemachines.com\/browse?show=MuForth" cr cr/
}

# while we're here, fix up copyright
s/(Copyright 199.-200)./\16/

