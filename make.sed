# $Id$

# sed script to convert a BSD makefile (yay!) to a GNU makefile (ewwww).

/Id: Makefile/c\
# This Makefile is automatically generated; do not edit! \
# Edit the Makefile.in and re-run configure.sh.

s/Makefile\.in/Makefile/

s/^.(ifdef|else|endif|include)/\1/

s/\{\.TARGET\}/@/g
s/\{\.ALLSRC\}/^/g
s/\{\.IMPSRC\}/</g
s/\{\.OODATE\}/?/g

/### BSD start ###/,/### BSD end ###/d
