# $Id$
# Make a link to nimblemachines in all the source code.

# This matches C comments, Makefile etc comments, and lines starting with
# zero or more spaces.
/^[ *#]*This file is part of muforth/ {
s/^([ *#]*)(This.*)muforth.$/\1\2muFORTH; for project details, visit\
\1\
\1\ \ \ http:\/\/nimblemachines.com\/browse?show=MuForth/
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

