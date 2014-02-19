#!/bin/sh

# Undo the effects of doing a sed by tossing the *.sedded files, without
# changing anything else (like copying over onto the original, like
# do_mv.sh would do).

[ -f $1.sedded ] && rm $1.sedded
