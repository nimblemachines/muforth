# change copyright & license

# This time, we're going from Apache to MIT; details are in the COPYRIGHT
# file, rather than being copied into each source file.

# Sadly, since I don't know a way to capture the line beginning style and
# then substitute that into the replacement string, I have to do this three
# times: once for Makefile/sh style comments; once for C style; and once for
# Forth style. Oh well.

# fix date - we're now in 2008! - and add link to COPYRIGHT file on next line.
# note that in Forth code we can't include the (c) because of the parens!
/Copyright.*David Frech/ {
s/(199.-200)./\18/
}
