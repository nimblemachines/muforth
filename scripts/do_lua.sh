#!/bin/sh

echo Processing $1

# Convert multi-line comments to new format.
lua scripts/comments.lua $2 < $1 > $1.edited
mv -f $1.edited $1
