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
# grab next
n
s/(reversed\.)/\1 (See the file COPYRIGHT for details.)/
# then delete following empty line
n
/^[#* ]*$/d
}

# We're just deleting the license text. This will match any comment style.
/^[#* ]*Licensed under the Apache License, Version 2.0/,/^[#* ]*limitations under the License./d

