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
DBGCFLAGS=-O0 -ggdb -Wall
ASFLAGS=-g
LDFLAGS=

# If any of these files changes, make a new version.h
ARCHX86  = i386.o i386_lib.o
ARCHPPC  = ppc.o ppc_asm.o
VERSOBJS = kernel.o interpret.o compile.o dict.o file.o \
	 error.o time.o pci.o tty.o select.o sort.o # buf.o

ALLOBJS = ${VERSOBJS} muforth.o

.PHONY: all clean

all : muforth

${ALLOBJS} : Makefile muforth.h

${ARCHX86} : Makefile muforth.h

${ARCHPPC} : Makefile muforth.h

muforth.o : version.h

version.h : Makefile ${VERSOBJS}
#	echo "struct counted_string version = COUNTED_STRING(\"${VERSION}\");" > version.h
	echo "#define VERSION \"${VERSION}\"" > version.h
	echo "time_t build_time = `date \"+%s\"`;" >> version.h

muforth : ${ALLOBJS} ${ARCHX86} gdb-i386.init
	${CC} ${LDFLAGS} -o $@ ${ALLOBJS} ${ARCHX86} ${LIBS}
	ln -sf gdb-i386.init .gdbinit

muforthppc : ${ALLOBJS} ${ARCHPPC} gdb-ppc.init
	${CC} ${LDFLAGS} -o $@ ${ALLOBJS} ${ARCHPPC} ${LIBS}
	ln -sf gdb-ppc.init .gdbinit

ppctest: ppc.c ppc_asm.s muforth.h
	${CC} ${DBGCFLAGS} -c -DPPC_TEST -o ppc.o ppc.c
	${CC} ${DBGCFLAGS} -c -DPPC_TEST -o ppc_asm.o ppc_asm.s
	${CC} ${LDFLAGS} -o ppctest ppc.o ppc_asm.o ${LIBS}

clean :
	rm -f muforth ppctest .gdbinit version.h *.o
