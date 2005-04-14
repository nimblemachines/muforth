/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2005 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/types.h>
#include <string.h>

#include "env.h"

typedef int cell;
typedef char int8;
typedef unsigned char uint8;

/* data stack */
#define STACK_SIZE 4096
#define STACK_SAFETY 256
#define S0  &stack[STACK_SIZE - STACK_SAFETY]
#define R0  &rstack[STACK_SIZE]

typedef void (*pw)(void);    /* ptr to word's code */
typedef pw *ppw;             /* ptr to ptr to word's code */
typedef ppw xtk;             /* "execution token" - ptr to ptr to code */

/* from mip, with changes */
extern cell  *SP;     /* parameter stack pointer */
extern cell   T;      /* top of stack */
extern xtk  **RP;     /* return stack pointer */
extern xtk   *IP;     /* instruction pointer */
extern xtk    W;      /* on entry, points to the current Forth word */

extern cell stack[];
extern xtk *rstack[];

#define SND      SP[0]
#define TRD      SP[1]
#define DUP      (*--SP = T)
#define NIP      (SP++)
#define NIP2     (SP += 2)
#define NIPN(n)  (SP += (n))
#define DROP     (T = SND, NIP)
#define DROP2    (T = TRD, NIP2)
#define DROPN(n) (T = SP[(n)-1], NIPN(n))

#define PUSH(n)  (DUP, T = (cell)(n))
#define POP      mu_pop_dstack()
#define EXEC(x)  (W = (xtk)(x), (*W)())
#define EXECUTE  EXEC(POP)
#define NEXT     EXEC(*IP++)
#define BRANCH   (IP = *(xtk **)IP)

#define RPUSH(n)  (*--RP = (xtk *)(n))
#define RPOP      (*RP++)

#define NEST     RPUSH(IP)
#define UNNEST   (IP = RPOP)

#ifdef DEBUG
#include <stdio.h>
#define BUG printf
#else
#define BUG
#endif

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
 * struct text is an odd beast. It's intended for parsing, and other
 * applications that scan a piece of text. To make this more efficient
 * we store a pointer to the _end_ of the text, and a _negative_
 * offset to its start, rather than the way struct string works.
 */

struct text
{
    char *end;
    ssize_t start;  /* ssize_t is a _signed_ type */
};

struct counted_string
{
    size_t length;  /* cell-sized length, unlike older Forths */
    char data[0];
};

extern struct string parsed;    /* for errors */

extern int  cmd_line_argc;
extern char **cmd_line_argv;

extern uint8 *pdt0;   /* ptrs to name & data spaces */
extern cell  *pcd0;   /* ptr to names & code space */

extern uint8 *pdt;    /* ptrs to next free byte in each space */
extern cell  *pcd;

/* XXX: Gross hack alert! */
extern char *ate_the_stack;
extern char *ate_the_rstack;
extern char *isnt_defined;
extern char *version;

/* declare common functions */

/* public.h is automagically generated, and can match every public function
 * taking no arguments. Other functions need to be put here explicitly.
 */
#include "public.h"

/* compile.c */
char *to_counted_string(char *);

/* error.c */
void die(const char *msg);

/* kernel.c */
int string_compare(const char *string1, size_t length1,
                   const char *string2, size_t length2);
