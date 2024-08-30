/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "env.h"

typedef  int64_t    cell;   /* stack or heap cell: always 64-bit */
typedef uint64_t   ucell;   /* unsigned cell - useful for some computations */

/* The addr type is used when interfacing with C pointer types. */
typedef uintptr_t  addr;    /* intptr_t and uintptr_t are integer types that
                                are the same size as a native pointer. Whether
                                this type is unsigned or not will affect how
                                32-bit addresses are treated when pushed onto
                                a 64-bit stack: signed will sign-extend,
                                unsigned will zero-extend. */

/* Forth VM registers */
extern cell  *SP;   /* parameter stack pointer */
extern cell  *RP;   /* return stack pointer */
extern cell  *IP;   /* instruction pointer; points to a cell */
extern cell  *W;    /* on entry, points to the current Forth word */

typedef void (*code)(void);     /* machine POINTER to word's machine code */

/* dictionary size */
/* Let's allocate 8MiB of heap. */
#define HEAP_CELLS      ((8 * 1024 * 1024) / sizeof(cell))

/* This is the alignment of cells in the heap. */
#define ALIGN_SIZE  sizeof(cell)
#define ALIGNED(x)  (((intptr_t)(x) + ALIGN_SIZE - 1) & -ALIGN_SIZE)

/*
 * Are pointers in the heap relative or absolute?
 *
 * Absolute is now the default, but it's easy to change. Just change the
 * following line to #define from #undef and re-run make.
 */
#undef RELATIVE_HEAP_ADDRESSES

#ifdef RELATIVE_HEAP_ADDRESSES

extern intptr_t heap;   /* pointer to start of heap space, as an integer value */

/*
 * HEAPIFY turns a host address into a heap-relative address;
 * UNHEAPIFY goes the other way, and casts to a (cell *) because that's
 * almost always what we want.
 */
#define HEAPIFY(host_addr)              ((addr)(host_addr) - heap)
#define UNHEAPIFY(heap_addr)    (cell *)(      (heap_addr) + heap)

extern void mu_do_colon();

/*
 * The code field contains an *offset* from mu_do_colon, which is
 * "cleverly" chosen to do two things:
 *
 *   (1) All code fields start with the digits "c0de", so they are easy to
 *   recognize when dumping the host heap.
 *
 *   (2) The code field for colon words is made to be especially
 *   recognizable. See the choice of "colon signatures" below.
 */

/* Take your pick! Or add your own! */
#define COLON_SIGNATURE_1   0xc0dec001  /* "code cool" */
#define COLON_SIGNATURE_2   0xc0dec010  /* c010 suggests "colon" */
#define COLON_SIGNATURE_3   0xc0decccc  /* visually easy to recognize */
#define COLON_SIGNATURE_4   0xc0dec01a  /* for fans of fizzy drinks */

#define CODE_OFFSET         (mu_do_colon + (-COLON_SIGNATURE_3))

#define CODE(f)           ((f) - CODE_OFFSET)
#define FUN(c)      (code)((c) + CODE_OFFSET)

#else

/* We are using *absolute* addresses in the heap. */
#define HEAPIFY(host_addr)        (addr)(host_addr)
#define UNHEAPIFY(heap_addr)    (cell *)(heap_addr)

#define CODE(f)     (addr)(f)
#define FUN(c)      (code)(c)

#endif  /* relative vs absolute heap addresses */

/* data and return stacks */
/* Both stacks are 64 bits wide! */
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
#define POP         (*SP++)
#define PUSH_ADDR(v)    PUSH(HEAPIFY(v))
#define POP_ADDR        UNHEAPIFY(POP)

/* Return stack */
#define RTOP        (*RP)
#define RDROP(n)    (RP += (n))
#define RPUSH(n)    (*--RP = (cell)(n))
#define RPOP        (*RP++)
#define RPUSH_ADDR(n)   RPUSH(HEAPIFY(n))
#define RPOP_ADDR       UNHEAPIFY(RPOP)

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
