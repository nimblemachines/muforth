# sed script to convert a BSD makefile (yay!) to a GNU makefile (ewwww).

# elide the leading . for directives
s/^.(ifdef|else|endif|include)/\1/

# use short ugly names for automatic variables
s/\{\.TARGET\}/@/g
s/\{\.ALLSRC\}/^/g
s/\{\.IMPSRC\}/</g
s/\{\.OODATE\}/?/g

/### BSD start ###/,/### BSD end ###/d
