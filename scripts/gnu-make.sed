# sed script to convert a BSD makefile (yay!) to a GNU makefile (ewwww).

# elide the leading . for directives and edit them accordingly
s/^\.(if|else|endif|s?include)/\1/

# BSD's sinclude GNU calls -include
s/^sinclude/-include/

# BSD requires filenames to be surrounded by "" or <> (just like #include)
s/^(-?include) ["<](.*)[">]/\1 \2/

# for if, always put the variable on the left, string on the right
s/^if (['"].*['"]) ([!=]=) (\$[({].*[)}])/\3 \2 \1/

# convert if $var == "string" to ifeq "$var" "string"
#     and if $var != "string" to ifneq "$var" "string"
s/^if (.*) == (.*)/ifeq "\1" \2/
s/^if (.*) != (.*)/ifneq "\1" \2/

# use short ugly names for automatic variables
s/\{\.TARGET\}/@/g
s/\{\.ALLSRC\}/^/g
s/\{\.IMPSRC\}/</g
s/\{\.OODATE\}/?/g

/### BSD start ###/,/### BSD end ###/d
