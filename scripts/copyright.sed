# update copyright

# note that in Forth code we can't include the (c) because of the parens!
/Copyright.*David Frech/ {
s/20(..)-20../20\1-2026/
}

# XXX now that we are using "|" for block comments, can I add the (c) to the
# Forth copyright comments?
s/^\| Copyright (20..)/| Copyright (c) \1/
