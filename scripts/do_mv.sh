#!/bin/sh

# copy the sedded version over the original file
[ -f $1.sedded ] && mv $1.sedded $1
