/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* dictionary */

#include "muforth.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Dictionary is one unified space, just like the old days. ;-)
 */

/*
 * Dictionary is kept cell-aligned.
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
 * there will be at least -one- address cell (on 32-bit machines, 3 bytes
 * and a length; on 64-bit machines, 7 bytes and a length) of name I will
 * define a struct that represents only the last three (or seven)
 * characters of the name, the length, and the link; the previous
 * characters are "off the map" as far as C is concerned, but there is an
 * easy way to address them. (To get to the beginning of a name, take the
 * address of the suffix, add the SUFFIX_LEN (3 or 7), and subtract the
 * length.)
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

/*
 * On 64-bit machines ADDR_ALIGN_SIZE is 8. On 32-bit machines, it is 4.
 */
#define SUFFIX_LEN  (ADDR_ALIGN_SIZE - 1)
struct dict_name
{
    char suffix[SUFFIX_LEN];     /* last 3 or 7 characters of name */
    unsigned char length;        /* 127 max; high bit = hidden */
    struct dict_name *link;      /* link to preceding dict_name */
};

/*
 * A struct dict_entry is the -whole- thing: a name plus a code field.
 * Having the two together makes writing mu_find much cleaner.
 */
struct dict_entry
{
    struct dict_name n;
    code code;
};

/*
 * The forth and compiler "vocabulary" chains
 *
 * Thes are now initialised at dictionary init time, by calling new_name()
 * with a name starting with a DEL character. This way these pseudo-words
 * won't show up when listed by  word  but _can_ be found (with care) using
 * find.
 */
static struct dict_name *forth_chain;
static struct dict_name *compiler_chain;

/* current chain to compile into */
static struct dict_name *current_chain;

/* hook called when a new name is created */
CODE(mu_nope)
static xtk xtk_new_hook = XTK(mu_nope);

/* bogus C-style dictionary init */
struct inm          /* "initial name" */
{
    char *name;
    code  code;
};

struct inm initial_forth[] = {
#include "forth_chain.h"
    { NULL, NULL }
};

struct inm initial_compiler[] = {
#include "compiler_chain.h"
    { NULL, NULL }
};

void mu_push_h0()       /* push address of start of dictionary */

{
    PUSH_ADDR(ph0);
}

void mu_here()          /* push current _value_ of heap pointer */
{
    PUSH_ADDR(ph);
}

/*
 * , (comma) copies the cell on the top of the stack into the dictionary,
 * and advances the heap pointer by one cell. Note that the ph is kept
 * cell-aligned, not addr-aligned!
 */
void mu_comma() { *ph++ = (cell)POP; }

/*
 * a, (a-comma) copies the address on the top of the stack into the
 * dictionary, and advances the heap pointer by one address. NOTE: this
 * operation assumes that the dictionary has the correct alignment for an
 * address. We assert that this is true.
 */
void mu_acomma()
{   
    assert(ADDR_ALIGNED(ph) == (intptr_t)ph, "misaligned (acomma)");

    *(addr *)ph = POP;
    ph += sizeof(addr)/sizeof(cell);
}

/*
 * allot ( n)
 *
 * Takes a count of bytes, rounds it up to a cell boundary, and adds it to
 * the heap pointer. Again, this keeps ph always aligned.
 */
void mu_allot() { ph += ALIGNED(POP) / sizeof(cell); }

/* Align TOP to cell boundary */
void mu_aligned() { TOP = ALIGNED(TOP); }

/*
 * NOTE: The value of current_chain is a pointer to a struct dict_name.
 * Here we are pushing its address since we're expecting to fetch or store
 * its value in Forth.
 */
void mu_push_current()
{
    PUSH_ADDR(&current_chain);
}

/*
 * .forth. and .compiler. push the address of the respective struct
 * dict_name.
 */
void mu_push_forth_chain()
{
    PUSH_ADDR(forth_chain);
}

void mu_push_compiler_chain()
{
    PUSH_ADDR(compiler_chain);
}

/* Type of string compare functions */
typedef int (*match_fn_t)(const char*, const char*, size_t);

/* Default at startup is case-sensitive */
match_fn_t match = (match_fn_t)memcmp;

/*
 * +case  -- make dictionary searches case-sensitive -- DEFAULT
 * -case  -- make dictionary searches case-insensitive
 */
void mu_plus_case()   { match = (match_fn_t)memcmp; }
void mu_minus_case()  { match = strncasecmp; }

/*
 * find takes a token (a u) and a chain (the head of a vocab word list) and
 * searches for the token on that chain. If found, it returns the address
 * of the code field of the word; if -not- found, it leaves the token (a u)
 * on the stack, and returns false (0).
 */
/* find  ( a u chain - a u 0 | code -1) */
void mu_find()
{
    char *token = (char *) ST2;
    cell length = ST1;
    struct dict_entry *pde = (struct dict_entry *)TOP;

    if (length < 128)
    {
        /*
         * Only search if length < 128. This prevents us from matching
         * hidden entries!
         */

        while ((pde = (struct dict_entry *)pde->n.link) != NULL)
        {
            /* for speed, don't test anything else unless lengths match */
            if (pde->n.length != length) continue;

            /* lengths match - compare strings */
            if ((*match)(pde->n.suffix + SUFFIX_LEN - length, token, length) != 0)
                continue;

            /* found: drop token, push code address and true flag */
            DROP(1);
            ST1 = (addr)&pde->code;
            TOP = -1;
            return;
        }
    }
    /* not found: leave token, push false */
    TOP = 0;
}

/*
 * new_name creates a new dictionary (name) entry and returns it
 */
static struct dict_name *new_name(
    struct dict_name *link, char *name, int length, int hidden)
{
    struct dict_name *pnm;  /* the new name */

    assert(ADDR_ALIGNED(ph) == (intptr_t)ph, "misaligned (new_name)");

    /*
     * Since we're using the high bit of the length as a "hidden" or
     * "deleted" flag, cap the length at 127.
     */
    length = MIN(length, 127);

    /* Allot space for name + length byte so that suffix is aligned. */
    pnm = (struct dict_name *)ADDR_ALIGNED((intptr_t)ph + length - SUFFIX_LEN);

    /* copy name string */
    memcpy(pnm->suffix + SUFFIX_LEN - length, name, length);
    pnm->length = length + (hidden ? 128 : 0);

    /* set link pointer */
    pnm->link = link;

    /* Allot entry */
    ph = (cell *)(pnm + 1);

#if defined(BEING_DEFINED)
    fprintf(stderr, "%p %p %.*s\n", pnm, link, length, name);
#endif

    return pnm;
}

/* (name)  ( link a u hidden - 'suffix) */
void mu_name_()
{
    struct dict_name *pnm = new_name(
        (struct dict_name *)ST3, (char *)ST2, (int)ST1, (int)TOP);
    DROP(3);
    TOP = (addr)pnm;
}

/*
 * new_linked_name creates a new dictionary (name) entry and links it onto
 * the chain represented by pnmHead.
 */
static void new_linked_name(
    struct dict_name *pnmHead, char *name, int length)
{
    /* create new name & link onto front of chain */
    pnmHead->link = new_name(pnmHead->link, name, length, 0);
}

/*
 * Called (indirectly, thru the mu_new* words) from Forth. Only creates a
 * name; does NOT set the code field!
 */
static void mu_new_name()
{
    new_linked_name(current_chain, (char *)ST1, TOP);
    DROP(2);
}

void mu_new_()
{
    execute_xtk(xtk_new_hook);
    mu_new_name();
}

void mu_new()
{
    mu_token();
    mu_new_();
}

void mu_push_tick_new_hook()
{
    PUSH_ADDR(&xtk_new_hook);
}

/*
 * All the words defined by this function are CODE words. Their bodies are
 * defined in C; this routine compiles a code pointer into the dict entry
 * that points to the C function.
 */
static void init_chain(struct dict_name *pchain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        new_linked_name(pchain, pinm->name, strlen(pinm->name));
        PUSH_ADDR(pinm->code);      /* set code pointer */
        mu_acomma();
    }
}

static void allocate()
{
    ph0 = (cell *) calloc(DICT_CELLS, sizeof(cell));

    if (ph0 == NULL)
        die("couldn't allocate memory");

    /* init heap pointer */
    ph = ph0;
}

void init_dict()
{
    allocate();

    /* create "hidden" names */
    forth_chain    = new_name( NULL, ".forth.", 7, 1);
    compiler_chain = new_name( NULL, ".compiler.", 10, 1);

    init_chain(forth_chain, initial_forth);
    init_chain(compiler_chain, initial_compiler);
    current_chain = forth_chain;
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
