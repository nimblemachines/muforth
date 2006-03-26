# $Id$

# sed script to mark a generated makefile so that it doesn't get edited.

/Id: Makefile/c\
# This Makefile is automatically generated; do not edit! \
# Edit the Makefile.in and re-run configure.sh.

s/Makefile\.in/Makefile/
