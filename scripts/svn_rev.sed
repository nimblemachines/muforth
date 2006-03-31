# $Id$

# Get the latest Subversion revision into muforth. The idea is that we keep
# modifying the file to keep up with the rev number; this way it always
# gets a new rev, and will always have the highest rev of any muforth
# source file.

/\$Revision:/ {
# get next line
N
s/^(.* )([0-9]+)( .*)\n(.*)/\1\2\3\
#define SVN_REV \2/
}
