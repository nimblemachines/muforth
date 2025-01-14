/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2025 David Frech. (Read the LICENSE for details.)
 */

/* Interpreter and compiler */

#include "muforth.h"

#include <ctype.h>

/* for debug */
#include <sys/uio.h>

#if defined (DEBUG_STACK) || defined(DEBUG_TOKEN)
#include <stdio.h>
#endif

/* C version of state variable */
static code state = &muboot_interpret_token;

/*
 * These are read by executing mu_source_fetch(), and written by executing
 * mu_source_store().
 */
static char *first;     /* goes from start to end of input source text */
static char *end;       /* end of input */

/* We read and write this from Forth using @ and !; it has to be a cell. */
static cell lineno;     /* line number - incremented for each newline */

int parsed_lineno;              /* captured with first character of token */
struct string parsed;           /* for errors */
static struct string skipped;   /* whitespace skipped before token */
static struct string trailing;  /* whitespace skipped after token */

/*
 * By defining source@ and source! we make it unnecessary to read or write
 * C pointer variables from Forth!
 */

/* source@  ( - end first) */
void mu_source_fetch()  { PUSH_ADDR(end); PUSH_ADDR(first) ; }

/* source!  ( end first) */
void mu_source_store()  { first = (char *)POP_ADDR; end = (char *)POP_ADDR; }

/* Push address of lineno, which we call "line" in Forth. */
void mu_push_line()     { PUSH_ADDR(&lineno); }

/* Push captured line number */
void mu_at_line()       { PUSH(parsed_lineno); }

void mu_push_parsed()
{
    PUSH_ADDR(parsed.data);
    PUSH(parsed.length);
}

void mu_empty_parsed()
{
    parsed.length = 0;
}

void mu_push_skipped()
{
    PUSH_ADDR(skipped.data);
    PUSH(skipped.length);
}

void mu_push_trailing()
{
    PUSH_ADDR(trailing.data);
    PUSH(trailing.length);
}

static void capture_token(char *last, int ate_trailing)
{
    /* Get address and length of the token */
    parsed.data = first;
    parsed.length = last - first;

    /* Save trailing delimiter as a token: address and length */
    trailing.data = last;
    trailing.length = ate_trailing;

    /* Account for characters processed */
    first = last + ate_trailing;

#ifdef DEBUG_TOKEN
    /* Without these casts, this doesn't work! */
    fprintf(stderr, "%.*s\n", (int)parsed.length, (char *)parsed.data);
#endif
}

/* Skip leading whitespace */
static void skip()
{
    /* Record skipped whitespace as if it's a token */
    skipped.data = first;

    while (first < end && isspace(*first))
    {
        if (*first == '\n') lineno++;
        first++;
    }

    skipped.length = first - skipped.data;
}

/*
 * Scan for trailing delimiter and consume it, unless we run out of
 * input text first.
 */
static void scan(int delim)
{
    char *last;
    char c;

    /* capture lineno that token begins on */
    parsed_lineno = lineno;

    for (last = first; last < end; last++)
    {
        c = *last;
        if (c == '\n') lineno++;
        if (delim == c
            || (delim == ' ' && isspace(c)))
        {
            /* found trailing delimiter; consume it */
            capture_token(last, 1);
            return;
        }
    }

    /* ran out of text; don't consume trailing */
    capture_token(last, 0);
}

void mu_token()  /* -- first len */
{
    skip();             /* skip leading whitespace */
    scan(' ');          /* scan for trailing whitespace and capture token */
    mu_push_parsed();   /* push parsed token */
}

void mu_parse()  /* delim -- first len */
{
    /* The first character of unseen input is the first character of token. */
    scan(POP);          /* scan for trailing delimiter and capture token */
    mu_push_parsed();   /* push parsed token */
}

/*
: complain   error"  is not defined"  -;
: huh?   if ^ then complain  ;   ( useful after find or token' )
defer not-defined  now complain is not-defined
*/

/* We're going to assume that in every case that complain is called, that
 * there is a token ( a u) on the stack. So complain will do a 2drop.
 */
void mu_complain()
{
    DROP(2);
    abort_zmsg("isn't defined");
}

void mu_huh_q()
{
    if (POP) return;
    mu_complain();
}

/* The interpreter's "consume" function. */
/* Not declared "static" because it is needed by muforth.c to fire up warm! */
void muboot_interpret_token()
{
    muboot_push_forth_chain();
    mu_find();
    if (POP)
    {
        mu_execute();
        return;
    }
    mu_complain();
}

/* The compiler's "consume" function. */
static void muboot_compile_token()
{
    muboot_push_compiler_chain();
    mu_find();
    if (POP)
    {
        mu_execute();
        return;
    }
    muboot_push_runtime_chain();
    mu_find();
    if (POP)
    {
        mu_compile_comma();
        return;
    }
    mu_complain();
}

void mu_compiler_lbracket()
{
    state = &muboot_interpret_token;
}

void mu_rbracket()
{
    state = &muboot_compile_token;
}

#ifdef DEBUG_STACK
#define SHOW_STACK muboot_show_stack()
static void muboot_show_stack()
{
    int i;
    fprintf(stderr, "  --");
    for (i = 0; i < 4; i++)
        fprintf(stderr, "  %16llx", (ucell)SP[3-i]);
    fprintf(stderr, "\n");
    fflush(stderr);
}
#else
#define SHOW_STACK
#endif

/*
 * This version of interpret is -not- exported to Forth! We are going to
 * re-define it in Forth so it executes as "pure" Forth, and we can use
 * Forth-side, return-stack-based exception handling!
 *
 * Oddly enough, the Forth implementation will be be *exactly* the same!
 * The only difference is that it will be executing in Forth, so we can use
 * R stack tricks to change its behaviour.
 */
static void muboot_interpret()
{
    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        (*state)();     /* consume token */
        SHOW_STACK;
    }
    DROP(2);
}

/*
 * This will also be re-implemented in Forth, but with nice nesting of
 * various context variables: radix, first, end, etc.
 */
void muboot_load_file()    /* c-string-name */
{
    int fd;

    mu_r_slash_o();     /* open file readonly */
    mu_open_file_q();
    mu_q_abort();
    fd = TOP;
    mu_read_file();

    first = (char *)UNHEAPIFY(ST1);
    end   = first + TOP;
    lineno = 1;
    DROP(2);

    muboot_interpret();

    PUSH(fd);
    mu_close_file();
}
