# $Id$

# sed script to convert a BSD makefile (yay!) to a GNU makefile (ewwww).

s/^.(ifdef|else|endif|include)/\1/

s/\{\.TARGET\}/@/g
s/\{\.ALLSRC\}/^/g
s/\{\.IMPSRC\}/</g
s/\{\.OODATE\}/?/g

/### BSD start ###/,/### BSD end ###/d
