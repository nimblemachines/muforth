# $Id$
#
# This file is part of muforth.
#
# Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
# reversed.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# or see the file LICENSE in the top directory of this distribution.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

###
### Makefile for muforth, a "micro" Forth
###

### 2004-jun-24. daf. Made this file as generic as possible, stripping out
### all the "web projects" stuff. Now it should compile with BSD make or
### GNU make.
 
VERSION = 0.01

CFLAGS=-O2 -Wall -fomit-frame-pointer
DBGCFLAGS=-O -ggdb -Wall -DDEBUG
ASFLAGS=-g
LDFLAGS=

# If any of these files changes, make a new version.h
VERSOBJS = kernel.o interpret.o compile.o dict.o file.o \
	 interpreter.o compiler.o library.o debugger.c float.o \
	 error.o time.o pci.o tty.o select.o sort.o # buf.o

ALLOBJS = ${VERSOBJS} muforth.o
DEPFILES = Makefile muforth.h opcode.h

.PHONY: all clean

all : muforth

${ALLOBJS} : ${DEPFILES}

muforth.o : version.h muforth.h

version.h : Makefile ${VERSOBJS}
	echo "#define VERSION \"${VERSION}\"" > version.h
	echo "time_t build_time = `date \"+%s\"`;" >> version.h

muforth : ${ALLOBJS} ${DEPFILES}
	${CC} ${LDFLAGS} ${CFLAGS} -o $@ ${ALLOBJS} ${LIBS}

clean :
	rm -f muforth version.h *.o

## For merging changes in other branches into HEAD
.if defined (BRANCH)

syncpoint !
	cvs rtag -r ${BRANCH} ${BRANCH}_merged_to_HEAD muforth

do_merge !
	cvs update -j ${BRANCH}_merged_to_HEAD -j ${BRANCH}

merge ! do_merge syncpoint

.else
.error You need to define a source BRANCH to merge from.
.endif
