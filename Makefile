###
### Makefile for muforth, a "micro" Forth
###

### Extensively modified 11-aug-1999 to make use of the fancy features from
### my web page Makefile. Extensively modified some more on that same day
### to extract common stuff to $HOME/Projects/Make.rules.

### This can't be in Make.rules!
ifeq (0,$(MAKELEVEL))
top	:= $(shell pwd)
endif

### Now include the per-directory variables
include Make.variables

### Project info
version = 0.01

### Utilities
M4 = gm4
CC = gcc
AS = as
LN = ln

### Options
CFLAGS=-O2 -Wall -fomit-frame-pointer
LDFLAGS=

### Lists
OBJS = muforth.o kernel.o i386.o interpret.o compile.o dict.o file.o \
	 error.o time.o pci.o tty.o select.o # buf.o
LSTS = $(OBJS:.o=.lst)
LIBS =
HFILES =

### Pattern rules ###

# $@ is the file name of the target of the rule.
# $< is the name of the first dependency.

%.s : %.S
	$(CC) -E -Wall $< -o $@

%.s : %.c
	$(CC) $(CFLAGS) -S $< -o $@

%.S : %.m4
	$(M4) --prefix-builtins $< > $@

#%.o : %.s
#	$(AS) $< -o $@

%.lst : %.s
	$(AS) $< -als=$@ -o junk
	rm -f junk

### The main deal. ###

.PHONY: all always

all : muforth  # subdirs

$(OBJS) : Makefile muforth.h

$(LSTS) : Makefile muforth.h

kernel.o : version.h

version.h : always
	echo "char version[] = \"$(version)\";" > version.h
	echo "time_t build_time = $(stamp);" >> version.h

muforth : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

lists : $(LSTS)


### include at the end so the default target is our main target, above
include $(top)/build-tools/Make.rules

### That's all, folks!
