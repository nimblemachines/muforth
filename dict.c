/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* dictionary */

#include "muforth.h"
#include <stdio.h>
#include <sys/mman.h>

/*
 * Dictionary is one unified space, just like the old days. ;-)
 */

cell  *ph0;     /* pointer to start of heap space */
cell  *ph;      /* ptr to next free byte in heap space */

/*
 * A struct dict_name represents what Forth folks often call a "head" - a
 * name, its length, and a link to the previous head (in the same
 * vocabulary).
 *
 * Note that this definition does -not- include the code field. All of the
 * dictionary code - except for mu_find - deals only with these "pure"
 * names.
 *
 * The way that names are stored is a bit odd, but has -huge- advantages.
 * Many Forths store the link, then a byte-count-prefixed name, then
 * padding, then the code field and the rest of the body of the word. This
 * works fine when mapping from names to code fields (or bodies) - which is
 * what one does 97% of the time.
 *
 * But the other 3% of the time you're holding a code field address, or the
 * address of the body of a word, and you wonder, "What word does this
 * belong to? What's its name?" (Doing decompilation is a great example of
 * this, but not the only one.) This kind of "reverse mapping" is
 * impossible with the above "naive" dictionary layout. The problem: it's
 * impossible to reliably skip -backwards- over the name. The length byte
 * is at the -beginning- of the name, but the code field follows it.
 *
 * There are a couple of hacks that have been used. One (adopted by
 * fig-Forth) is to set the high bit of both the length byte and the last
 * byte of the name. This way one can skip backwards over the name, but
 * it's a slow scanning process, rather than an arithmetic one (subtracting
 * the length from a pointer).
 *
 * Another (ugly) hack is to put the length at -both- ends of the name.
 * This obviously makes it easy to skip over the name, forwards or
 * backwards.
 *
 * A much nicer solution is the move the link field -after- the name, and
 * forgo the prefix length entirely.  This is the solution that muFORTH
 * adopts (as dforth did before it).  Instead of -following- the link
 * field, the name -precedes- it, starting with padding, then the
 * characters of the name, then its length (one byte), followed directly by
 * the link. (The padding puts the link on a cell boundary.)
 *
 * Experienced C programmers will immediately notice a problem: structs
 * (such as the one we want to use to define the structure of dictionary
 * entries) cannot begin with a variable-length field. (In standard C they
 * cannot even -end- with variable-length fields - or that's my understand.
 * There are evil GCC-only workarounds, however.) So, what do we do? We'd
 * like to profitably use C's structs to help write readable and bug-free
 * code, but how?
 *
 * My solution is a bit weird, but it works rather well. Since I know that
 * there will be at least -one- cell (3 bytes and a length) of name (but
 * likely more), I will define a struct that represents only the last three
 * characters of the name, the length, and the link; the previous
 * characters are "off the map" as far as C is concerned, but there is an
 * easy way to address them. (To get to the beginning of a name, take the
 * address of the last three characters - suffix - add three and subtract
 * the length.)
 *
 * The only part of the code that seems a bit hackish is the calculation of
 * how many bytes to allocate for the name to put the link on a cell
 * boundary.
 *
 * One quirk of this method is that links no longer point to links, and
 * this forced me to restructure some code. In particular, .forth.  and
 * .compiler. (and all other vocab chains) are no longer simply
 * single-celled organisms (variables); they are now fully-fledged struct
 * name's. (One neat advantage to this is that it works well when chaining
 * vocabs together.)
 */
struct dict_name
{
    char suffix[3];              /* last 3 characters of name */
    unsigned char length;        /* true length of name */
    struct dict_name *link;      /* to preceding dict_name */
};

/*
 * A struct dict_entry is the -whole- thing: a name plus a code field.
 * Having the two together makes writing mu_find much cleaner.
 */
struct dict_entry
{
    struct dict_name n;
    pw code;
};

/* The indexes of the .forth. & .compiler chains. */
#define FORTH_CHAIN     0
#define COMPILER_CHAIN  1

/* current chain to compile into */
static int current_chain = FORTH_CHAIN;

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

void mu_push_h0()       /* start of head (dictionary) */
{
    PUSH(ph0);
}

void mu_push_h()        /* heap pointer */
{
    PUSH(&ph);
}

/*
 * NOTE: The value of current_chain is a pointer to a struct dict_name.
 * Here we are pushing its address since we're expecting to fetch or store
 * its value in Forth.
 */
void mu_push_current()
{
    PUSH(&current_chain);
}

/*
 * .forth. and .compiler. push the address of the respective struct
 * dict_name.
 */
void mu_push_forth_chain()
{
    PUSH(FORTH_CHAIN);    /* index of .forth. */
}

void mu_push_compiler_chain()
{
    PUSH(COMPILER_CHAIN);    /* index of .compiler. */
}

/*
 * We now hash dictionary chains 32 ways. What this means is that there is
 * an array of 32 "heads", and the chains, instead of being pointers, are
 * indexes (from 0 to 31). To figure out which list to search, you "hash"
 * the first character of the name and the index, like this:
 *    hash = (chain_index + first_char) mod 32
 */
struct dict_name dict_heads[32];
#define HEAD(n)     (&dict_heads[(n)])

void mu_head()  /* index -- head */
{
    TOP = (cell)HEAD(TOP);
}

/*
 * The real HASH function. Adds the first character, the length, and the
 * chain index, and returns an index mod 32. This allows at most 32
 * distinct "vocab chains". Given its simplicity (and speed), the
 * distribution is surprisingly good.
 */
#define HASH(name, len, chain)  ((*(char *)(name) + (len) + (chain)) & 31)

/*
 * This is the HASH function I tried the first time. It doesn't take into
 * account the length of the word (just its first letter and the chain
 * index). It worked, but the distribution isn't nearly as good the above
 * function - on the core word set, two "buckets" were empty, and one (#3)
 * had over 40 words on it - the words starting with '#' or 'c' in the
 * .forth. chain.
 */
//#define HASH(name, len, chain)  ((*(char *)(name) + (chain)) & 31)

/*
 * For testing, here is a "trivial" HASH function that puts the entirety of
 * a vocab chain onto a single list. The head is simply HEAD(chain-index).
 */
//#define HASH(name, len, chain)  ((chain) & 31)

void mu_hash()  /* name length chain -- name length hash-index */
{   
    TOP = (cell)HASH(ST2, ST1, TOP);
}

/*
 * find takes a token (a u) and a chain (the head of a vocab word list) and
 * searches for the token on that chain. If found, it returns the address
 * of the code field of the word; if -not- found, it leaves the token (a u)
 * on the stack, and returns false (0).
 */
/* find  ( a u chain - a u 0 | code -1) */
void mu_find()
{
    char *token = (char *)ST2;
    cell length = ST1;
    struct dict_entry *pde;

    /* hash first character of token with chain# */
    pde = (struct dict_entry *)HEAD(HASH(token, length, TOP));

    while ((pde = (struct dict_entry *)pde->n.link) != NULL)
    {
        if (pde->n.length != length) continue;
        if (memcmp(pde->n.suffix + 3 - length, token, length) != 0) continue;

        /* found: drop token, push code address and true flag */
        DROP(1);
        ST1 = (cell)&pde->code;
        TOP = -1;
        return;
    }
    /* not found: leave token, push false */
    TOP = 0;
}

/*
 * make_new_name creates a new dictionary (name) entry, and links it onto
 * the chain represented by pnmHead.
 */
static void make_new_name(
    int chain, char *name, int length)
{
    struct dict_name *pnmHead;
    struct dict_name *pnm;              /* the new name */
    char *pch = (char *)ALIGNED(ph);    /* start out nicely aligned */

    /* hash first character of token with chain# */
    pnmHead = HEAD(HASH(name, length, chain));

    /* Allocate extra cells for name, if longer than 3 characters */
    if (length > 3)
    {
        int cchExtra = ALIGNED(length - 3);
        pch += cchExtra;
    }

    pnm = (struct dict_name *)pch;

    pnm->link = pnmHead->link;          /* compile link to last */
    pnmHead->link = pnm;                /* link onto front of chain */
    pnm->length = length;
    memcpy(pnm->suffix + 3 - length, name, length);        /* copy name string */

    /* Allot entry */
    ph = (cell *)(pnm + 1);

#if defined(BEING_DEFINED)
    fprintf(stderr, "%2d %p %.*s\n", HASH(name, length, chain), ph, length, name);
#endif
}

/*
 * Called (indirectly, thru the mu_new* words) from Forth. Only creates a
 * name; does NOT set the code field!
 */
static void mu_make_new_name()
{
    make_new_name(current_chain, (char *)ST1, TOP);
    DROP(2);
}

void mu_new_()
{
    execute_xtk(xtk_new_hook);
    mu_make_new_name();
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
static void init_chain(int chain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        make_new_name(chain, pinm->name, strlen(pinm->name));
        *ph++ = (cell)pinm->code;   /* set code pointer */
    }
}

static void allocate()
{
    ph0 = (cell *)  mmap(0, 1024 * 4096, PROT_READ | PROT_WRITE,
                            MAP_ANON | MAP_PRIVATE, -1, 0);

    if (ph0 == MAP_FAILED)
        die("couldn't allocate memory");

    /* init heap pointer */
    ph = ph0;
}

void init_dict()
{
    allocate();
    init_chain(FORTH_CHAIN, initial_forth);
    init_chain(COMPILER_CHAIN, initial_compiler);
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
