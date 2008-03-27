/*
 * This file is part of muFORTH: http://pages.nimblemachines.com/muforth
 *
 * Copyright (c) 2002-2008 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* dictionary */

#include "muforth.h"
#include <stdio.h>

/*
 * Names currently live in _code_ space, rather than in their own name space.
 */

struct dict_entry
{
    struct dict_entry *link;
    unsigned char length;
    char name[0];
};

/* the forth and compiler "vocabulary" chains */
static struct dict_entry *forth_chain = NULL;
static struct dict_entry *compiler_chain = NULL;

/* current chain to compile into */
static struct dict_entry **current_chain = &forth_chain;

/* pointer to latest word defined; doesn't depend on current *still*
 * pointing to the chain it *was* pointing to when the word was
 * defined... */
static struct dict_entry *latest = NULL;

/* hook called when a new name is created */
static xtk xtk_new_hook = XTK(mu_nope);

/* bogus C-style dictionary init */
struct inm          /* "initial name" */
{
    char *name;
    pw   code;
};

struct inm initial_forth[] = {
#include "forth_chain.h"
    { NULL, NULL }
};

struct inm initial_compiler[] = {
#include "compiler_chain.h"
    { NULL, NULL }
};

/*
 * NOTE: The value of current_chain is a pointer to a variable (like
 * forth_chain) that itself points to a struct dict_entry. Here we are
 * pushing its address since we're expecting to fetch or store its value in
 * Forth.
 */
void mu_push_current()
{
    PUSH(&current_chain);
}

/*
 * .forth. and .compiler. push the address of the respective variable,
 * since the first thing the dictionary search code does is dereference
 * the pointer to get the first struct dict_entry.
 *
 * Actually the truth is more disgusting than that. mu_find expects a
 * (struct dict_entry *) on the stack. When we start looking down a
 * chain, we initially push a pointer to a pointer, which looks like
 * the struct, which - conveniently - has its link pointer as the first
 * entry, so "p->link" is essentially "*p". It's ugly but it works.
 */
void mu_push_forth_chain()
{
    PUSH(&forth_chain);
}

void mu_push_compiler_chain()
{
    PUSH(&compiler_chain);
}

/*
 * NOTE: Though "latest" is a variable, we never want to store into it,
 * so push its _value_ rather than its _address_.
 */
void mu_push_latest()
{
    PUSH(latest);
}

/*
 * 2004-apr-01. After giving a talk on muforth to SVFIG, and in particular
 * after Randy asked me some pointed questions, I decided that find should
 * have positive logic after all. I have renamed -"find to find to indicate
 * this change.
 */
/* find  ( a u chain - a u 0 | code -1) */
void mu_find()
{
    char *token = (char *) ST2;
    cell length = ST1;
    struct dict_entry *pde = (struct dict_entry *) TOP;

    while ((pde = pde->link) != NULL)
    {
        if (pde->length != length) continue;
        if (memcmp(pde->name, token, length) != 0) continue;

        /* found: drop token, push code address and true flag */
        NIP(1);
        ST1 = (cell) ALIGNED(pde->name + length);
        TOP = -1;
        return;
    }
    /* not found: leave token, push false */
    TOP = 0;
}

static void compile_dict_entry(
    struct dict_entry **ppde, char *name, int length)
{
    struct dict_entry *pde;

    pde = (struct dict_entry *)ALIGNED(pcd);

    pde->link = *ppde;      /* compile link to last */
    *ppde = pde;        /* link onto front of chain */
    pde->length = length;
    bcopy(name, pde->name, length);          /* copy name string */

    /* Allot entry */
    pcd = (cell *)ALIGNED(pde->name + length);

    /* Account for its size */
    names_size += (caddr_t)pcd - (caddr_t)pde;

#if defined(BEING_DEFINED)
    fprintf(stderr, "%p %.*s\n", pcd, length, name);
#endif
}

/* Called (indirectly, thru the mu_new* words) from Forth. Only creates a
 * name; does NOT set the code field!
 *
 * Note also: the old way of recalling the last word defined by doing
 * "current @ @" to get "latest" doesn't work if between defining and
 * querying you switch what current points to (which you do ALL THE TIME
 * while meta-compiling!). So this is fragile. Better to explicitly capture
 * the address of the last word defined - pointed to indirectly by
 * "current_chain".
 */
static void mu_compile_name()
{
    compile_dict_entry(current_chain, (char *)ST1, TOP);
    latest = *current_chain;  /* remember the last defined */
    DROP(2);
}

void mu_new_()
{
    execute_xtk(xtk_new_hook);
    mu_compile_name();
}

void mu_new()
{
    mu_token();
    mu_new_();
}

void mu_push_tick_new_hook()
{
    PUSH(&xtk_new_hook);
}

static void init_chain(struct dict_entry **pchain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        compile_dict_entry(pchain, pinm->name, strlen(pinm->name));
        *pcd++ = (cell)pinm->code;  /* set code pointer */
    }
}

void init_dict()
{
    init_chain(&forth_chain, initial_forth);
    init_chain(&compiler_chain, initial_compiler);
}

/*
   We're going to take some pointers from Martin Tracy and zenFORTH.
   `last-link' is a 2variable that stores (link, chain) of the last
   named word defined.  We don't actually link it into the dictionary
   until we think it's safe to do so, thereby obviating the need for
   `smudge'.

   2variable last-link  ( &link &chain)
   variable last-code   ( pointer to last-compiled code field)
   variable last-word   ( last compiled word)

   : show     last-link 2@  ( &link chain)  !   ;
   : use      ( code -)  last-code @ !  ;
   : patch    pop @  use   ;

   : ?unique  ( a u - a u)
   2dup current @  ?unique-hook  find  if
   drop  2dup  fd-out @ push  >stderr  type  ."  again.  "  pop writes
   then   2drop ;

   : code,   0 , ( lex)  here last-code !  0 , ( code)  ;
   : token,  ( - 'link)  token  ?unique  here  scrabble>  allot  ;
   : link,   ( here)  current @  dup @ ,  last-link 2!  ;
   : head,   token,  link,  ;

   : name        head,  code,  ;
   : noname   0 align,  code,  ;

   ( Dictionary structure)
   : link>name   1- ( char-)  dup c@  swap over -  swap  ;
   : >link  ( body - link)  cell- cell- cell- ;
   : link>  ( link - body)  cell+ cell+ cell+ ;
   : >name  ( body - a u)   >link  link>name  ;
   : >lex   ( body - lex)   cell- cell-  ;
   : >code  ( body - code)  cell-  ;
*/
