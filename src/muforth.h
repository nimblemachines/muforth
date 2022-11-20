/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2022 David Frech. (Read the LICENSE for details.)
 */

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "env.h"

#ifdef MU_ADDR_32
    #define CELL_SHIFT 2
    #define MUCHAIN_CELLS 2
#else
    #define CELL_SHIFT 3
    #define MUCHAIN_CELLS 1
#endif

/* Heap cells are the size of a pointer; stack values are 64 bits. */
typedef uintptr_t   cell;   /* heap cell */
typedef  int64_t    val;    /* stack value - data or return stack */
typedef uint64_t   uval;    /* unsigned stack value */

/* address type - for casting */
typedef uintptr_t  addr;    /* intptr_t and uintptr_t are integer types that
                                are the same size as a native pointer. Whether
                                this type is unsigned or not will affect how
                                32-bit addresses are treated when pushed onto
                                a 64-bit stack: signed will sign-extend,
                                unsigned will zero-extend. */

/* pointer and cell types */

typedef void (*code)(void);     /* POINTER to word's machine code */
typedef code *xt;               /* POINTER to code field */

/* Forth VM execution registers */
extern val  *SP;    /* parameter stack pointer */
extern val  *RP;    /* return stack pointer */
extern xt   *IP;    /* instruction pointer; points to an xt */
extern xt    W;     /* on entry, points to the current Forth word */

/* dictionary size */
/* Let's allocate 8MiB of heap, regardless of cell size. */
#define HEAP_CELLS      ((8 * 1024 * 1024) / sizeof(cell))
/* XXX heap or h0 ? */
extern cell *heap;  /* pointer to start of heap space */

/* data and return stacks */
/* NOTE: Even on 32-bit platforms the R stack is 64 bits wide! This makes
 * things _much_ simpler, at the expense of a bit more storage. */
extern val dstack[];
extern val rstack[];

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
#define PUSH(v)     (*--SP = (val)(v))
#define PUSH_ADDR(v)    PUSH((addr)(v))
#define POP         (*SP++)

/* Return stack */
#define RTOP        (*RP)
#define RDROP(n)    (RP += (n))
#define RPUSH(n)    (*--RP = (val)(n))
#define RPOP        (*RP++)

#define ALIGN_SIZE  sizeof(cell)
#define ALIGNED(x)  (((intptr_t)(x) + ALIGN_SIZE - 1) & -ALIGN_SIZE)

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
