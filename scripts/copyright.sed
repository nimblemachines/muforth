# update copyright

# fix date - we're now in 2008! - and add link to COPYRIGHT file on next line.
# note that in Forth code we can't include the (c) because of the parens!
/Copyright.*David Frech/ {
s/(199.-200)./\18/
}
