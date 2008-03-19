# Make a link to nimblemachines in all the source code.

# put a link into the startup banner of startup.mu4
/\( ### httplink ###/ {
# get next line
N
s/^(.*)\n(.*)/\1\
\ ."\ \ \ http:\/\/pages.nimblemachines.com\/muforth" cr cr/
}

# convert three line banner with old URL to new one-liner
/^[#* ]*This file is part of muFORTH; for project details, visit/ {
N
N
s/^([#* ]*).*/\1This file is part of muFORTH: http:\/\/pages.nimblemachines.com\/muforth/
}

# for any files which still refer to "muforth", convert them and put it a
# one-liner banner.

# This matches C comments, Makefile etc comments, and lines starting with
# zero or more spaces.
s/^([ *#]*This file is part of )muforth\.$/\1muFORTH: http:\/\/pages.nimblemachines.com\/muforth/
