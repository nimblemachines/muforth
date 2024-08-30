/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* Nuts and bolts of threaded-code compiler & execution engine */

#include "muforth.h"

/*
 * This is where all the magic happens. Any time we have the address of a
 * word to execute (an execution token, or xt) - either because we looked
 * it up in the dictionary, or because we fetched it out of a variable - we
 * call mu_execute().
 *
 * We need magic here because of the hybrid nature of the system. It is
 * *not* a truly indirect-threaded Forth, where you set up IP and go, and
 * never look back. We're executing C code, then Forth, then C... The Forth
 * inner interpreter never runs "free"; in fact, we run it in this routine!
 *
 * How do we know if the xt refers to a Forth word or to a C routine? We
 * don't...until we run it! If it's Forth, the first thing it will do is
 * NEST - push the caller's IP - and then it will set IP to point to its
 * own code. In fact, that's *all* Forth words do when you execute them!
 *
 * So we save RP, then call the word. Then we loop *while* the current RP
 * is *strictly* less than the saved. If we didn't call a Forth word, this
 * will not be true, and we'll exit. But the word we called - the C routine
 * - will have run its course and done all its work.
 *
 * If it *was* a Forth word, it NESTed, and RP < rp_saved. So now we run
 * NEXT - a "constrained" threaded interpreter - until RP >= rp_saved, which
 * will happen precisely when the called (Forth) word executes UNNEST
 * (EXIT).
 *
 * That's all there is to it!
 *
 * Thanks to Michael Pruemm for the idea of comparing RP to rp_saved as a
 * way to see when we're "done".
 */

#define CALL(xt)  (W = UNHEAPIFY(xt), (*FUN(*W++))())

void mu_execute()
{
    cell *rp_saved;

    rp_saved = RP;

    CALL(POP);              /* pop stack and execute xt */
    while (RP < rp_saved)
        CALL(*IP++);        /* do NEXT */
}

#define NEST      RPUSH_ADDR(IP)
#define UNNEST    (IP = RPOP_ADDR)

/* The most important "word" of all: */
void mu_do_colon()
{
    NEST;       /* entering a new word; push IP */
    IP = W;     /* new IP is address of parameter field */
}

/* The basis of create/does>. */
static void mu_do_does()
{
    NEST;                   /* entering a new word; push IP */
    IP = UNHEAPIFY(*W++);   /* new IP is stored in the parameter field */
    PUSH_ADDR(W);           /* push the address of the word's body */
}

/* Compile an xt into the dictionary. Even though this is simply a synonym
 * for mu_comma(), we are keeping it as a separate word because it makes
 * clearer what is being "comma'd" into the heap. */
void mu_compile_comma()     { mu_comma(); }

void mu_set_colon_code()    { PUSH(CODE(mu_do_colon)); mu_compile_comma(); }
void mu_set_does_code()     { PUSH(CODE(mu_do_does)); mu_compile_comma(); }

/* Normal exit */
void mu_runtime_exit()      { UNNEST; }

/* Push an inline 64-bit literal. */
void mu_runtime_lit_()      { PUSH(*IP++); }

/* Compile the following word by treating it as a literal value and then
 * comma-ing it into the heap. */
void mu_runtime_compile()   { mu_runtime_lit_(); mu_compile_comma(); }


/*
 * These are the control structure runtime workhorses.
 */
#define BRANCH    (IP = UNHEAPIFY(*IP))
#define SKIP      (IP++)

void mu_runtime_branch_()           { BRANCH; }
void mu_runtime_equal_0branch_()    { if (TOP == 0) BRANCH; else SKIP; }
void mu_runtime_0branch_()          { mu_runtime_equal_0branch_(); DROP(1); }
void mu_runtime_q0branch_()         { if (TOP == 0) { BRANCH; DROP(1); } else SKIP; }

/*
 * (next)
 *
 * "for" now compiles (?0branch) followed by push and doesn't need a
 * matching "then" after "next"; "next" compiles (next) and automatically
 * does the "then".
 *
 * At run-time (ie when (?0branch) executes), if TOP is zero we skip the
 * entire loop by DROPping the zero and jumping to after "next"; otherwise
 * we skip the branch offset, push the count onto the R stack, and continue
 * into the body of the for/next loop.
 *
 * Without this test for zero we could be looping for a long time -
 * 2^(#bits in CELL)!
 */

void mu_runtime_next_()
{
    if (--RTOP == 0)        /* decrement counter on top of R stack */
        { SKIP; RP++; }     /* zero: skip branch, pop counter */
    else
        { BRANCH; }         /* non-zero: branch back */
}

/*
 * I decided it can't hurt to support do/loop/+loop. Lots of people
 * understand it, and folks might have "legacy" code that they don't want
 * to rewrite into for/next.
 *
 * do/loop isn't a bad construct. I think really the only reason that Chuck
 * deprecated it - starting in cmForth - was that it didn't map neatly to
 * his chip! The runtime words (do) (loop) and (+loop) need two - or three!
 * - items on the stack. His original stack computer could only see the top
 * slot of the return stack. Hence, for/next, and simple count-down loops.
 *
 * We are going to implement a "zero-crossing" version. For some reason
 * many Forths choose to offset the start and limit values so that the loop
 * exits when the index "crosses" from the most negative number (8000...
 * hex) to the most positive (7fff... hex) - ie, when arithmetic overflow
 * occurs. I can't remember why this seemed like a good idea. I'm going to
 * implement a version that offsets the index so that the limit is always
 * 0. We'll see how that works.
 */

void mu_runtime_do_()   /* (do)  ( limit start) */
{
    RPUSH(*IP++);               /* push following branch address for (leave) */
    RPUSH(ST1);                 /* limit */
    RPUSH(TOP - ST1);           /* index = start - limit */
    DROP(2);
}

void mu_runtime_loop_()
{
    RTOP++;                     /* increment index on top of R stack */
    if (RTOP == 0)
        { SKIP; RP += 3; }      /* zero: skip branch, pop R stack */
    else
        { BRANCH; }             /* non-zero: branch back */
}

void mu_runtime_plus_loop_()    /* (+loop)  ( incr) */
{
    cell prev = RTOP;

    RTOP += TOP;                /* increment index */
    if ((RTOP ^ prev) < 0)      /* current & prev index have opposite signs */
        { SKIP; RP += 3; }      /* opposite: skip branch, pop R stack */
    else
        { BRANCH; }             /* same: branch back */
    DROP(1);
}

/* leave the do loop early */
void mu_runtime_leave()
{
    IP = UNHEAPIFY(RP[2]);      /* jump to address saved on R stack */
    RP += 3;                    /* pop "do" context */
}

/* conditionally leave */
void mu_runtime_qleave()
{
    if (POP) mu_runtime_leave();
}

/*
 * Calculate current loop indices for current (i), enclosing (j), and
 * third-level (k) do-loops
 */

void mu_runtime_i()  { PUSH(RP[0] + RP[1]); }
void mu_runtime_j()  { PUSH(RP[3] + RP[4]); }
void mu_runtime_k()  { PUSH(RP[6] + RP[7]); }

/* R stack functions */
void mu_runtime_rfetch()   { PUSH(RP[0]); }
void mu_runtime_to_r()     { RPUSH(POP); }
void mu_runtime_r_from()   { PUSH(RPOP); }

void mu_runtime_2rfetch()  { PUSH(RP[1]); PUSH(RP[0]); }
void mu_runtime_2to_r()    { RPUSH(SP[1]); RPUSH(SP[0]); DROP(2); }
void mu_runtime_2r_from()  { mu_runtime_2rfetch(); RDROP(2); }

/* These are Chuck's newfangled names for >r and r> */
/* XXX Consider deprecating and removing these? */
void mu_runtime_push()   { mu_runtime_to_r(); }
void mu_runtime_pop()    { mu_runtime_r_from(); }

/* Similarly, 2push and 2pop are synonyms for 2>r and 2r>, resp. */
void mu_runtime_2push()   { mu_runtime_2to_r(); }
void mu_runtime_2pop()    { mu_runtime_2r_from(); }

/* rdrop is shorthand for r> drop */
void mu_runtime_rdrop()   { RDROP(1); }
void mu_runtime_2rdrop()  { RDROP(2); }

/*
 * I'm beginning to prefer rdrop, but until I change a bunch of existing
 * code, let's make shunt a synonym for rdrop.
 */
void mu_runtime_shunt()  { mu_runtime_rdrop(); }


#if 0
/*
 * Using these two words - r@+ and ?^ - it would be easy to implement the
 * branch runtime words in Forth rather than in C.
 */

/* Whether positive or negative logic can easily be changed; whether it
 * consumes TOP can be changed as well, depending on what seems to work
 * best. However, unnest-if-false should probably be called -?^ */

void mu_runtime_qexit()  { if (TOP) UNNEST; DROP(1); }

/* So we can easily define compile and (lit) in Forth. Fetches and pushes
 * to D stack top value on R stack; increments that value by sizeof(cell).
 * That yield an IP pointer on D stack; to get the _value_ we need to
 * fetch. This fetch could be built into the word, and maybe renamed to
 * r@+@ or something equally ungainly. Or maybe it could get a more
 * mnemonic (what it means), rather than functional (what it does) name.
 */
static void mu_runtime_rfetch_plus()  { PUSH(RP[0]); RP[0]++; }
#endif

