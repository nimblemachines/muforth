/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2019 David Frech. (Read the LICENSE for details.)
 */

/* dictionary */

#include "muforth.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Dictionary is one unified space, just like the old days. ;-)
 */

/*
 * Dictionary is kept cell-aligned. Cells are 64 bits.
 */
static cell  *ph0;  /* pointer to start of heap space */
       cell  *ph;   /* ptr to next free byte in heap space */

/*
 * A struct dict_name represents what Forth folks often call a "head" - a
 * name, its length, and a link to the previous head (in the same
 * vocabulary).
 *
 * Note that this definition does -not- include the code field. All of the
 * dictionary code - except for mu_find() and init_chain() - deals only
 * with these "pure" names.
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
 * A much nicer solution is to move the link field -after- the name, and
 * forgo the prefix length entirely. This is the solution that muforth
 * adopts (as dforth did before it). Instead of -following- the link field,
 * the name -precedes- it, starting with padding, then the characters of
 * the name, then its length (one byte), followed directly by the link.
 * (The padding puts the link on a cell boundary.)
 *
 * Experienced C programmers will immediately notice a problem: structs
 * (such as the one we want to use to define the structure of dictionary
 * entries) cannot begin with a variable-length field. (In standard C they
 * cannot even -end- with variable-length fields - or that's my
 * understanding. There are evil GCC-only workarounds, however.) So, what
 * do we do? We'd like to profitably use C's structs to help write readable
 * and bug-free code, but how?
 *
 * My solution is a bit weird, but it works rather well. Since I know that
 * there will be at least -one- cell (7 bytes and a length) of name I will
 * define a struct that represents only the last seven characters of the
 * name, the length, and the link; the previous characters are "off the
 * map" as far as C is concerned, but there is an easy way to address them.
 * (To get to the beginning of a name, take the address of the suffix, add
 * the SUFFIX_LEN (7), and subtract the length.)
 *
 * Links still point to links; adjustments are made in a couple of places
 * to convert a pointer to a link into a pointer to a name entry (struct
 * dict_name).
 *
 *  .forth.  and  .compiler.  (and all other vocab chains) are no longer
 * simply single-celled organisms (variables); they are now fully-fledged
 * struct name's. (One neat advantage to this is that it works well when
 * chaining vocabs together.)
 */


/* This is the only way C will let us define a type that points to itself. */
struct link_field
{
    CELL_T(struct link_field *) cell;
};

typedef struct link_field link_cell;


/*
 * Cells are 64 bits, so ALIGN_SIZE is 8.
 */
#define SUFFIX_LEN  (ALIGN_SIZE - 1)

struct dict_name
{
    char suffix[SUFFIX_LEN];    /* last 7 characters of name */
    unsigned char length;       /* 127 max; high bit = hidden */
    link_cell link;             /* link to preceding link */
};

/*
 * A struct dict_entry is the -whole- thing: a name plus a code field.
 * Having the two together makes writing mu_find much cleaner.
 */
struct dict_entry
{
    struct dict_name n;
    code_cell code;
};

/*
 * The forth and compiler "vocabulary" chains
 *
 * These are now initialised at dictionary init time by calling new_name()
 * with the hidden flag true. This way these pseudo-words won't be found by
 * mu_find() or show up when listed by  word .
 */
static link_cell *forth_chain;
static link_cell *compiler_chain;
static link_cell *runtime_chain;

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

struct inm initial_runtime[] = {
#include "runtime_chain.h"
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
 * Even though we are now populating the dictionary with "proper"
 * muchain-style .forth. .compiler. and .runtime. , we still need a way for
 * the code in interpret.c to push the addresses of these chains. Hence the
 * three following words.
 */
void muboot_push_forth_chain()
{
    PUSH_ADDR(forth_chain);
}

void muboot_push_compiler_chain()
{
    PUSH_ADDR(compiler_chain);
}

void muboot_push_runtime_chain()
{
    PUSH_ADDR(runtime_chain);
}

/*
 * , (comma) copies the cell on the top of the stack into the dictionary,
 * and advances the heap pointer by one cell. Note that ph is kept
 * cell-aligned.
 */
void mu_comma()
{
    assert(ALIGNED(ph) == (intptr_t)ph, "misaligned (comma)");
    *ph++ = (cell)POP;
}

/*
 * allot ( n)
 *
 * Takes a count of bytes, rounds it up to a cell boundary, and adds it to
 * the heap pointer. Again, this keeps ph always aligned.
 */
void mu_allot()    { ph += ALIGNED(POP) / sizeof(cell); }

/* Align TOP to cell boundary */
void mu_aligned()  { TOP = ALIGNED(TOP); }

/* Type of string compare functions */
typedef int (*match_fn_t)(const char*, const char*, size_t);

/* Default at startup is case-sensitive */
match_fn_t match = strncmp;

/*
 * +case  -- make dictionary searches case-sensitive -- DEFAULT
 * -case  -- make dictionary searches case-insensitive
 */
void mu_plus_case()   { match = strncmp; }
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
    struct dict_entry *pde;
    link_cell *plink = (link_cell *)TOP;

    /*
     * Only search if length < 128. This prevents us from matching hidden
     * entries!
     */
    if (length < 128)
    {
        while ((plink = _(plink->cell)) != NULL)
        {
            /* convert pointer to link to pointer to suffix */
            pde = (struct dict_entry *)(plink - 1);

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
static link_cell *new_name(
    link_cell *link, char *name, int length, int hidden)
{
    struct dict_name *pnm;  /* the new name */
    int prefix_bytes;

    assert(ALIGNED(ph) == (intptr_t)ph, "misaligned (new_name)");

    /*
     * Since we're using the high bit of the length as a "hidden" or
     * "deleted" flag, cap the length at 127.
     */
    length = MIN(length, 127);

    /*
     * Calculate space for the bytes of the name that don't fit into
     * suffix[]; align to cell boundary.
     */
    prefix_bytes = ALIGNED(length - SUFFIX_LEN);

    /*
     * Zero name + link + code. While dict started out zeroed, use of pad
     * could have put all kinds of gunk into it.
     */
    memset(ph, 0, prefix_bytes + sizeof(struct dict_entry));

    /* Allot prefix bytes and get a pointer to name struct. */
    pnm = (struct dict_name *)((intptr_t)ph + prefix_bytes);

    /* Copy name string */
    memcpy(pnm->suffix + SUFFIX_LEN - length, name, length);
    pnm->length = length + (hidden ? 128 : 0);

    /* Set link pointer */
    _(pnm->link.cell) = link;

    /* Allot entry */
    ph = (cell *)(pnm + 1);

#if defined(BEING_DEFINED)
    fprintf(stderr, "%p %p %.*s\n", &pnm->link, link, length, name);
#endif

    /* Return address of link field */
    return &pnm->link;
}

/*
 * new_linked_name creates a new, non-hidden dictionary (name) entry and
 * links it onto the chain represented by plink.
 */
static void new_linked_name(
    link_cell *plink, char *name, int length)
{
    /* create new name & link onto front of chain */
    _(plink->cell) = new_name(_(plink->cell), name, length, 0);
}

/* (linked-name)  ( a u chain) */
void mu_linked_name_()
{
    new_linked_name((link_cell *)TOP, (char *)ST2, ST1);
    DROP(3);
}

/*
 * All the words defined by this function are CODE words. Their bodies are
 * defined in C; this routine compiles a code pointer into the dict entry
 * that points to the C function.
 */
static void init_chain(link_cell *plink, link_cell *anchor_link, struct inm *pinm)
{
    /*
     * Set the beginning, "anchor", link for the chain. For sealed chains,
     * this will be NULL. For chains anchored to other chains, this will be
     * address of the link field of the chain being anchored to.
     */
    _(plink->cell) = anchor_link;

    for (; pinm->name != NULL; pinm++)
    {
        new_linked_name(plink, pinm->name, strlen(pinm->name));
        *ph++ = (addr)pinm->code;   /* set code pointer */
    }
}

/*
 * Create a chain that looks and behaves exactly like the chains that we
 * will be creating in forth using create/does>.
 *
 * Chains live in two worlds. In one world, they are normal words - with
 * names like .forth. or .compiler. - and they have a link and code fields
 * and a body.
 *
 * In the other world, they act as pointers to the head of the vocab chain
 * that they represent. In order make the chains "chainable" - so that
 * searches of one can continue in the chained chain - the body of a chain
 * needs to look like a dict entry. Thus, it has a (hidden) name - muchain
 * - and its link field points to the last word defined on its chain.
 *
 * To make this all work, these "natively-defined" chains need a code field
 * that points to code that pushes the address of the link field in the
 * hidden name. The following word will do this for us.
 */
static void mu_push_chain()
{
    PUSH_ADDR(&W[2]);   /* push the address of muchain's link field */
}

/*
 * NOTE: This is *only* called from init_dict() in order to create the initial three
 * chains. It is never called from Forth.
 *
 * Create a new chain.
 *
 * First, call new_linked_name() to create the public name: .forth. or
 * .compiler. or .runtime. . Then, populate its code field with code to
 * push the address of the following hidden word's link field. And, lastly,
 * create the hidden name ("muchain"), and return the address of the hidden
 * name's link field.
 *
 * plink points to the chain that this chain is _named_ in. It will always
 * be the .forth. chain.
 */
static link_cell *new_chain(
    link_cell *plink, char *name)
{
    new_linked_name(plink, name, strlen(name));
    *ph++ = (addr)mu_push_chain;    /* set code pointer */
    return new_name(NULL, "muchain", 7, 1);
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
    link_cell forth_bootstrap;  /* we need this to "bootstrap" the .forth. chain */

    allocate();

    /* First, populate the "bootstrap" .forth. chain with words defined in C. */
    init_chain(&forth_bootstrap, NULL, initial_forth);

    /*
     * Next, create in our "bootstrap" forth chain .forth. , .compiler. ,
     * and .runtime. chains that look and smell like the ones that will
     * later be created as create/does words. These have a body that looks
     * like a normal word, but the name is always "muchain" and the hidden
     * bit is set.
     */
    forth_chain    = new_chain(&forth_bootstrap, ".forth.");
    compiler_chain = new_chain(&forth_bootstrap, ".compiler.");
    runtime_chain  = new_chain(&forth_bootstrap, ".runtime.");

    /*
     * Now that everything is in the bootstrap .forth. chain, set the link
     * pointer in the real .forth. chain to match.
     */
    _(forth_chain->cell) = _(forth_bootstrap.cell);

    /* Now we can populate the .compiler. chain with words defined in C. */
    init_chain(compiler_chain, NULL, initial_compiler);

    /*
     * And we do the same with the .runtime. chain - with a wrinkle. Unlike
     * .forth. and .compiler. which are "sealed" chains, .runtime. is
     * anchored to the .forth. chain, so searches of .runtime. continue in
     * the .forth. chain.
     */
    init_chain(runtime_chain, forth_chain, initial_runtime);
}
