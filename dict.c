/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2009 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* dictionary */

#include "muforth.h"
#include <stdio.h>
#include <sys/mman.h>

/*
 * Names currently live in _code_ space, rather than in their own name space.
 */

int   names_size;   /* count of bytes alloted to names */

cell  *pcd0;   /* pointer to start of code & names space */
uint8 *pdt0;   /* ... data space */

cell  *pcd;    /* ptrs to next free byte in code & names space */
uint8 *pdt;    /* ... data space */

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

void mu_push_h()        /* "here" code space pointer */
{
    PUSH(&pcd);
}

void mu_push_r()        /* "ram" space pointer */
{
    PUSH(&pdt);
}

/*
 * Compile (that is, *copy*) a cell into the current code space.
 */
void mu_code_comma()     { *pcd++ = POP; }

void mu_push_code_size()
{
    PUSH(((caddr_t)pcd - (caddr_t)pcd0) - names_size);
}

void mu_push_names_size()
{
    PUSH(names_size);
}

void mu_push_data_size()
{
    PUSH(pdt - pdt0);
}

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

/* the char *string param here does _not_ need to be zero-terminated!! */
static char *compile_counted_string(char *string, size_t length)
{
    struct counted_string *cs;
    struct string s;

    cs = (struct counted_string *)pdt;
    s.data = string;
    s.length = length;
    cs->length = s.length;  /* prefix count cell */
    bcopy(s.data, &cs->data[0], s.length);
    cs->data[s.length] = 0; /* zero terminate */
    return &cs->data[0];
}

/*
 * copy string; return a counted string (addr of first character;
 * prefix count cell _precedes_ first character of string).
 * This does _not_ allot space in the dictionary!
 */
void mu_scrabble()  /* ( a u - z") */
{
    TOP = (cell)compile_counted_string((char *)ST1, TOP);
    NIP(1);
}

/*
 * (find) returns _both_ the name - the address of the first character, to
 * be precise - and the code pointer. Most code will want to use find
 * instead, which only returns the code field when a word is found in the
 * dictionary.
 *
 * I added this capability when I realised I wanted a cheap & cheerful way
 * to undefine (delete) words in the dictionary without mucking about with
 * link pointers. By blanking out the first byte of the name we can easily
 * achieve this.
 *
 * But in order to do that we have to have access to the name. Hence this
 * change.
 */
/* (find)  ( a u chain - a u 0 | code name -1) */
void mu_find_()
{
    char *token = (char *) ST2;
    cell length = ST1;
    struct dict_entry *pde = (struct dict_entry *) TOP;

    while ((pde = pde->link) != NULL)
    {
        if (pde->length != length) continue;
        if (memcmp(pde->name, token, length) != 0) continue;
        if (pde->name[0] == ' ') continue;      /* deleted */

        /* found: drop token, push code address, name, and true flag */
        ST2 = (cell) ALIGNED(pde->name + length);
        ST1 = (cell) pde->name;
        TOP = -1;
        return;
    }
    /* not found: leave token, push false */
    TOP = 0;
}

/*
 * This is the "normal" find, and what most code will want to use. But by
 * using (find) instead it's possible to get access to the name as well as
 * the code field.
 */
/* find  ( a u chain - a u 0 | code -1) */
void mu_find()
{
    mu_find_();
    if (TOP) NIP(1);
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

/*
 * All the words defined by this function are CODE words. Their bodies are
 * defined in C; this routine compiles a code pointer into the dict entry
 * that points to the C function.
 */
static void init_chain(struct dict_entry **pchain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        compile_dict_entry(pchain, pinm->name, strlen(pinm->name));
        *pcd++ = (cell)pinm->code;  /* set code pointer */
    }
}

static void allocate()
{
    pcd0 = (cell *)  mmap(0, 256 * 4096, PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE, -1, 0);

    pdt0 = (uint8 *) mmap(0, 1024 * 4096, PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE, -1, 0);

    if (pcd0 == MAP_FAILED || pdt0 == MAP_FAILED)
        die("couldn't allocate memory");

    /* init compiler ptrs */
    pcd = pcd0;
    pdt = pdt0;
}

void init_dict()
{
    allocate();
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
