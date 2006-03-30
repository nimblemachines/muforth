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

