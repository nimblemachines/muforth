# update copyright

# note that in Forth code we can't include the (c) because of the parens!
/Copyright.*David Frech/ {
#s/199.-/2002-/
#s/-200./-2021/
#s/-201./-2021/
s/2002-202./2002-2023/
}
