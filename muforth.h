/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "env.h"

typedef  int32_t  cell;
typedef uint32_t ucell;
typedef struct {
     cell hi;       /* Forth puts high cell at lower address (top of stack) */
    ucell lo;
} dcell;

/* data stack */
#define STACK_SIZE  4096
#define STACK_SAFETY  32
extern cell stack[];
#define S0    &stack[STACK_SIZE - STACK_SAFETY]
#define SMAX  &stack[STACK_SAFETY]

/* TOP is a synonym for ST0 */
#define TOP   ST0
#define ST0   SP[0]
#define ST1   SP[1]
#define ST2   SP[2]
#define ST3   SP[3]

#define DROP(n)  (SP += (n))
#define PUSH(v)  (*--SP = (cell)(v))
#define POP      (*SP++)

typedef void (*pw)(void);    /* ptr to word's machine code */
typedef pw    *ppw;          /* ptr to ptr to word's code */
typedef ppw    xtk;          /* "execution token" - ptr to ptr to code */

/* from mip, with changes */
extern cell  *SP;     /* parameter stack pointer */
extern xtk   *IP;     /* instruction pointer */
extern xtk    W;      /* on entry, points to the current Forth word */

/* return stack */
extern xtk *rstack[];
extern xtk  **RP;     /* return stack pointer */
#define R0  &rstack[STACK_SIZE]

/*
 * For a handful of words defined in the C kernel we need indirect code
 * pointers. These have been compiled into the dictionary, but to use those
 * we have to search for them; so we simply define them. They are
 * conventionally named "p_<mu_name>".
 */
#define XTK(w)   (&p_ ## w)   /* make an execution token from a word's name */

#define EXECUTE   execute_xtk((xtk)POP)
#define CALL(x)   (W = (xtk)(x), (**W)())
#define NEXT      CALL(*IP++)
#define BRANCH    (IP = *(xtk **)IP)

#define RPUSH(n)  (*--RP = (xtk *)(n))
#define RPOP      (*RP++)

#define NEST      RPUSH(IP)
#define UNNEST    (IP = RPOP)

#define ALIGN_SIZE  sizeof(cell)
#define ALIGNED(x)  (((cell)(x) + ALIGN_SIZE - 1) & -ALIGN_SIZE)

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

/*
 * struct text is intended for parsing, and other applications that scan a
 * piece of text. Because we will be scanning the text but also making sure
 * we don't run off the end, we store a pointer to the first character
 * (start) and a pointer to the first character not belonging to the text
 * (end). This way we can increment start and check that its less than end.
 * If we used struct string instead we'd have to keep adding length to data
 * to see if we've overrun.
 */
struct text
{
    char *start;
    char *end;
};

struct counted_string
{
    size_t length;  /* cell-sized length, unlike older Forths */
    char data[0];
};

extern struct string parsed;    /* for errors */
extern char *zloading;          /* the file we're currently loading; C string */

extern cell  *ph0;     /* pointer to start of heap space */
extern cell  *ph;      /* ptr to next free byte in heap space */

/* declare common functions */

/* public.h is automagically generated, and can match every public function
 * taking no arguments. Other functions need to be put here explicitly.
 */
#include "public.h"

/* engine-itc.c */
void execute_xtk(xtk x);

/* compile.c */
char *to_counted_string(char *);

/* error.c */
void die(const char *msg);
void throw(const char *zstring);

/* kernel.c */
int string_compare(const char *string1, size_t length1,
                   const char *string2, size_t length2);
