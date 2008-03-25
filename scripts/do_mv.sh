#!/bin/sh

# If they differ, copy the sedded version over the original file, otherwise
# delete the .sedded version.

if [ -f $1.sedded ]; then
	if diff $1 $1.sedded > /dev/null; then
		rm $1.sedded
	else
		mv $1.sedded $1
	fi
fi

