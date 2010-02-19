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
 * Names currently live in _code_ space, rather than in their own name space.
 */

int   names_size;   /* count of bytes alloted to names */

cell  *pcd0;   /* pointer to start of code & names space */
uint8 *pdt0;   /* ... data space */

cell  *pcd;    /* ptrs to next free byte in code & names space */
uint8 *pdt;    /* ... data space */

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

/*
 * The forth and compiler "vocabulary" chains - VH means vocab head.
 * Because the name field contains the three-character name " VH", these
 * will show up in a word list, if another vocab chains to one of these;
 * however, the names cannot easily be matched because they contain a
 * space.
 */
static struct dict_name forth_chain    = { " VH", 3, NULL };
static struct dict_name compiler_chain = { " VH", 3, NULL };

/* current chain to compile into */
static struct dict_name *current_chain = &forth_chain;

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
    PUSH(&forth_chain);
}

void mu_push_compiler_chain()
{
    PUSH(&compiler_chain);
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
    char *token = (char *) ST2;
    cell length = ST1;
    struct dict_entry *pde = (struct dict_entry *) TOP;

    while ((pde = (struct dict_entry *)pde->n.link) != NULL)
    {
        if (pde->n.length != length) continue;
        if (memcmp(pde->n.suffix + 3 - length, token, length) != 0) continue;

        /* found: drop token, push code address and true flag */
        NIP(1);
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
    struct dict_name *pnmHead, char *name, int length)
{
    struct dict_name *pnm;              /* the new name */
    char *pch = (char *)ALIGNED(pcd);   /* start out nicely aligned */

    /* Allocate extra cells for name, if longer than 3 characters */
    if (length > 3)
    {
        int cchExtra = ALIGNED(length - 3);
        pch += cchExtra;
        names_size += cchExtra;         /* count extra bytes alloc'ed */
    }

    pnm = (struct dict_name *)pch;

    pnm->link = pnmHead->link;          /* compile link to last */
    pnmHead->link = pnm;                /* link onto front of chain */
    pnm->length = length;
    bcopy(name, pnm->suffix + 3 - length, length);        /* copy name string */

    /* Allot entry */
    pcd = (cell *)(pnm + 1);

    /* Account for its size */
    names_size += (caddr_t)pcd - (caddr_t)pnm;

#if defined(BEING_DEFINED)
    fprintf(stderr, "%p %p %.*s\n", pnmHead, pcd, length, name);
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
static void init_chain(struct dict_name *pchain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        make_new_name(pchain, pinm->name, strlen(pinm->name));
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
