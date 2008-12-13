#!/bin/sh

# copy the sedded version over the original file

# Be careful with scripts; if the orig was executable, mv will clobber that. So
# we use cat instead. ;-)

[ -f $1.sedded ] && cat $1.sedded > $1 && rm $1.sedded
