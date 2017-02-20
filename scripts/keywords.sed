# Makefiles, sed, etc
/^# \$Id\$$/ {
N
/^# \$Id\$\n#$/d
}

# C files
/^ \* \$Id\$$/ {
N
/^ \* \$Id\$\n \*$/d
}

# text files
/^ *\$Id\$$/ {
N
/^ *\$Id\$\n *$/d
}

# Forth files
# this case is more complicated; since the $Id line starts with a "(", and
# since we're deleting that line, we have to put a leading "(" on the
# following non-blank line. Hard to do. But the following (non-blank) line is
# always "This file is part of muforth" etc.
/^\( \$Id\$$/ {
N
N
s/^\( \$Id\$\n\n  (This file is part of)/( \1/
}
