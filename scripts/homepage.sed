# Make a link to muforth home page in all the source code.

# Changing the homepage again! This time to a new and spiffy domain:
# muforth.dev !

s#muforth\.nimblemachines\.com#muforth.dev#

# skip the rest - it's only of historical interest
b skip

# We are now serving content over HTTPS!

s/This file is part of muforth: http:/This file is part of muforth: https:/

# The most egregious: fix the banner line in every source file
# to say "muforth" rather than "muFORTH".
s/This file is part of muFORTH:/This file is part of muforth:/

# I think every file has good links; now we just need to keep them updated.
s#http://sites\.nimblemachines\.com/muforth#http://muforth.dev/#
s#http://pages\.nimblemachines\.com/muforth/#http://muforth.dev/#

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
s/^([#* ]*).*/\1This file is part of muforth: https:\/\/pages.nimblemachines.com\/muforth/
}

# for any files which still refer to "muforth", convert them and put it a
# one-liner banner.

# This matches C comments, Makefile etc comments, and lines starting with
# zero or more spaces.
s/^([ *#]*This file is part of )muforth\.$/\1muFORTH: http:\/\/pages.nimblemachines.com\/muforth/

: skip

