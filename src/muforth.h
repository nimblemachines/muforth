/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2022 David Frech. (Read the LICENSE for details.)
 */

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "env.h"

/*
 * Heap addrs are the size of a machine pointer; stack and variable values
 * - aka cells - are 64 bits, regardless of architecture.
 */

typedef uintptr_t  addr;    /* intptr_t and uintptr_t are integer types that
                                are the same size as a native pointer. Whether
                                this type is unsigned or not will affect how
                                32-bit addresses are treated when pushed onto
                                a 64-bit stack: signed will sign-extend,
                                unsigned will zero-extend. */

typedef  int64_t    cell;    /* stack or variable value */
typedef uint64_t   ucell;    /* unsigned cell */

/* pointer types - these fit into an addr */
typedef void (*code)(void);     /* POINTER to word's machine code */
typedef code *xt;               /* POINTER to code field */

/* Forth VM execution registers */
extern cell  *SP;   /* parameter stack pointer */
extern cell  *RP;   /* return stack pointer */
extern xt    *IP;   /* instruction pointer; points to an xt */
extern xt     W;    /* on entry, points to the current Forth word */

/* dictionary size */
/* Let's allocate 8MiB of heap, regardless of addr size. */
#define HEAP_ADDRS      ((8 * 1024 * 1024) / sizeof(addr))
/* XXX heap or h0 ? */
extern addr *heap;  /* pointer to start of heap space */

/* data and return stacks */
/* NOTE: Even on 32-bit platforms the R stack is 64 bits wide! This makes
 * things _much_ simpler, at the expense of a bit more storage. */
extern cell dstack[];
extern cell rstack[];

#define STACK_SIZE  4096
#define STACK_SAFETY  32

/* Stack bottoms */
#define SP0   &dstack[STACK_SIZE - STACK_SAFETY]
#define RP0   &rstack[STACK_SIZE - STACK_SAFETY]

/* Data stack */
#define TOP   ST0
#define ST0   SP[0]
#define ST1   SP[1]
#define ST2   SP[2]
#define ST3   SP[3]

#define DROP(n)     (SP += (n))
#define PUSH(v)     (*--SP = (cell)(v))
#define PUSH_ADDR(v)    PUSH((addr)(v))
#define POP         (*SP++)

/* Return stack */
#define RTOP        (*RP)
#define RDROP(n)    (RP += (n))
#define RPUSH(n)    (*--RP = (cell)(n))
#define RPOP        (*RP++)

/* This is the alignment of *addrs* in the heap, *not* cell-sized things! */
#define ADDR_ALIGN_SIZE  sizeof(addr)
#define ADDR_ALIGNED(x)  (((intptr_t)(x) + ADDR_ALIGN_SIZE - 1) & -ADDR_ALIGN_SIZE)

/* This is the alignment of *cells*. */
#define CELL_ALIGN_SIZE  sizeof(cell)
#define CELL_ALIGNED(x)  (((intptr_t)(x) + CELL_ALIGN_SIZE - 1) & -CELL_ALIGN_SIZE)

/*
 * struct string is a "normal" string: pointer to the first character, and
 * length.
 */
struct string
{
    char *data;
    size_t length;
};

extern int parsed_lineno;       /* captured with first character of token */
extern struct string parsed;    /* last token parsed */

/* declare common functions */

/* public.h is automagically generated, and can match every public function
 * taking no arguments. Other functions need to be put here explicitly.
 */
#include "public.h"

/* error.c */
void die(const char *zmsg);
void abort_zmsg(const char *zmsg);
void assert(int cond, const char *zmsg);

/* file.c */
char *string_copy(char *dest, char *src);
char *concat_paths(char *dest, size_t destsize, char *p1, char *p2);
ssize_t read_carefully(int fd, void *buffer, size_t len);
void write_carefully(int fd, void *buffer, size_t len);

/* Utility macros */
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
