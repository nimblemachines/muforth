# $Id$
#
# This file is part of muforth.
#
# Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
# reversed.

###
### Makefile for muforth, a "micro" Forth
###

### 2004-jun-24. daf. Made this file as generic as possible, stripping out
### all the "web projects" stuff. Now it should compile with BSD make or
### GNU make.
 
VERSION = 0.01

CFLAGS=-O2 -Wall -fomit-frame-pointer
LDFLAGS=

# If any of these files changes, make a new version.h
VERSOBJS = muforth.o i386.o interpret.o compile.o dict.o file.o \
	 error.o time.o pci.o tty.o select.o # buf.o
ALLOBJS = ${VERSOBJS} kernel.o

.PHONY: all clean

all : muforth

${ALLOBJS} : Makefile muforth.h

kernel.o : version.h

version.h : Makefile ${VERSOBJS}
	echo "struct counted_string version = COUNTED_STRING(\"${VERSION}\");" > version.h
	echo "time_t build_time = `date \"+%s\"`;" >> version.h

muforth : ${ALLOBJS}
	${CC} ${LDFLAGS} -o $@ ${ALLOBJS} ${LIBS}

clean :
	rm -f muforth version.h *.o
