/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "env.h"

/* data cells - big enough to hold an address */
typedef  intptr_t   cell;
typedef uintptr_t  ucell;

/* stack cells - "values" - always 64 bits! */
typedef  int64_t   val;
typedef uint64_t  uval;

/* address type - for casting */
typedef intptr_t  addr;   /* intptr_t and uintptr_t are integer types that
                             are the same size as a native pointer. Whether
                             this type is unsigned or not will affect how
                             32-bit addresses are treated when pushed onto
                             a 64-bit stack: signed will sign-extend,
                             unsigned will zero-extend. */

/* dictionary size */
/* Cells are the native word size. Let's allocate 1M cells - 4MB of heap on
 * 32-bit machines, 8MB on 64-bit. */
#define DICT_CELLS     (1024 * 1024)

/* data stack */
#define STACK_SIZE  4096
#define STACK_SAFETY  32
extern val stack[];
#define S0    &stack[STACK_SIZE - STACK_SAFETY]
#define SMAX  &stack[STACK_SAFETY]

/* TOP is a synonym for ST0 */
#define TOP   ST0
#define ST0   SP[0]
#define ST1   SP[1]
#define ST2   SP[2]
#define ST3   SP[3]

#define DROP(n)  (SP += (n))
#define PUSH(v)  (*--SP = (val)(v))
#define PUSH_ADDR(v)  PUSH((addr)(v))
#define POP      (*SP++)

/* pointer and cell types */
typedef void (*code)(void);     /* POINTER to word's machine code */
typedef code  *xtk;             /* POINTER to code; aka "execution token" */

/* Forth VM execution registers */
extern val  *SP;    /* parameter stack pointer */
extern xtk  *IP;    /* instruction pointer */
extern xtk   W;     /* on entry, points to the current Forth word */

/* return stack */
/* NOTE: Even on 32-bit platforms the R stack is 64 bits wide! This makes
 * things _much_ simpler, at the expense of a bit more storage. */
extern val  rstack[];
extern val  *RP;    /* return stack pointer */
#define R0  &rstack[STACK_SIZE]

/*
 * For a handful of words defined in the C kernel we need indirect code
 * pointers. These have been compiled into the dictionary, but to use those
 * we have to search for them; so we simply define them. They are
 * conventionally named "p_<mu_name>".
 */

/* declare a code field for a word */
#define CODE(w)  code p_ ## w = &(w);

/* make an execution token from a word's name */
#define XTK(w)   (&p_ ## w)

#define EXECUTE   execute_xtk((xtk)POP)
#define CALL(x)   (W = (x), (**W)())
#define NEXT      CALL(*IP++)
#define BRANCH    (IP = (xtk *)*IP)

#define RDROP(n)  (RP += (n))
#define RPUSH(n)  (*--RP = (val)(n))
#define RPOP      (*RP++)

#define NEST      RPUSH((addr)IP)
#define UNNEST    (IP = (xtk *)RPOP)

#define ALIGN_SIZE  sizeof(cell)
#define ALIGNED(x)  (((intptr_t)(x) + ALIGN_SIZE - 1) & -(ALIGN_SIZE))

/*
 * struct string is a "normal" string: pointer to the first character, and
 * length. However, since these are often sitting on the data stack with
 * the length on top (and therefore at a _lower_ address) let's define it
 * that way.
 */
struct string
{
    size_t length;
    char *data;
};

struct counted_string
{
    cell length;    /* cell-sized length, unlike older Forths */
    char data[0];
};

extern int parsed_lineno;       /* captured with first character of token */
extern struct string parsed;    /* for errors */

extern cell  *ph0;     /* pointer to start of heap space */
extern cell  *ph;      /* ptr to next free byte in heap space */

/* declare common functions */

/* public.h is automagically generated, and can match every public function
 * taking no arguments. Other functions need to be put here explicitly.
 */
#include "public.h"

/* engine-itc.c */
void execute_xtk(xtk x);

/* error.c */
void die(const char *zmsg);
void abort_zmsg(const char *zmsg);
void assert(int cond, const char *zmsg);

/* kernel.c */
int string_compare(const char *string1, size_t length1,
                   const char *string2, size_t length2);

/* file.c */
char *path_prefix(char *src, char *dest, char sep, char *begin);

/* Utility macros */
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
