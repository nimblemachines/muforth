/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2019 David Frech. (Read the LICENSE for details.)
 */

/* Interpreter and compiler */

#include "muforth.h"

#include <ctype.h>

/* for debug */
#include <sys/uio.h>

#if defined (DEBUG_STACK) || defined(DEBUG_TOKEN)
#include <stdio.h>
#endif

/* interpreter mode */
struct imode
{
    xtk_cell eat;       /* consume one token */
    xtk_cell prompt;    /* display a mode-specific prompt */
};

/* cell versions of char * pointers */
typedef CELL_T(char *) charptr_cell;

static charptr_cell start;      /* input source text */
static charptr_cell end;
static charptr_cell first;      /* goes from start to end */

/* line number - incremented for each newline */
static cell lineno = 1;

int parsed_lineno;              /* captured with first character of token */
struct string parsed;           /* for errors */
static struct string skipped;   /* whitespace skipped before token */
static struct string trailing;  /* whitespace skipped after token */

/* Push lineno variable */
void mu_push_line()
{
    PUSH_ADDR(&lineno);
}

/* Push captured line number */
void mu_at_line()
{
    PUSH(parsed_lineno);
}

void mu_push_first()
{
    PUSH_ADDR(&first);
}

void mu_push_start()
{
    PUSH_ADDR(&start);
}

void mu_push_end()
{
    PUSH_ADDR(&end);
}

void mu_push_parsed()
{
    PUSH((addr) parsed.data);
    PUSH(parsed.length);
}

void mu_push_skipped()
{
    PUSH((addr) skipped.data);
    PUSH(skipped.length);
}

void mu_push_trailing()
{
    PUSH((addr) trailing.data);
    PUSH(trailing.length);
}

static void capture_token(char *last, int ate_trailing)
{
    /* Get address and length of the token */
    parsed.data = _(first);
    parsed.length = last - _(first);

    /* Save trailing delimiter as a token: address and length */
    trailing.data = last;
    trailing.length = ate_trailing;

    /* Account for characters processed */
    _(first) = last + ate_trailing;

#ifdef DEBUG_TOKEN
    /* Without these casts, this doesn't work! */
    fprintf(stderr, "%.*s\n", (int)parsed.length, (char *)parsed.data);
#endif
}

/* Skip leading whitespace */
static void skip()
{
    /* Record skipped whitespace as if it's a token */
    skipped.data = _(first);

    while (_(first) < _(end) && isspace(*_(first)))
    {
        if (*_(first) == '\n') lineno++;
        _(first)++;
    }

    skipped.length = _(first) - skipped.data;
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

    for (last = _(first); last < _(end); last++)
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

void mu_token()  /* -- start len */
{
    skip();         /* skip leading whitespace */
    scan(' ');   /* scan for trailing whitespace and capture token */
    mu_push_parsed();   /* push parsed token */
}

void mu_parse()  /* delim -- start len */
{
    /* The first character of unseen input is the first character of token. */
    scan(POP);   /* scan for trailing delimiter and capture token */
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
        EXECUTE;
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
        EXECUTE;
        return;
    }
    muboot_push_runtime_chain();
    mu_find();
    if (POP)
    {
        mu_comma();
        return;
    }
    mu_complain();
}

/*
 * Remember that the second part of a struct imode is a pointer to code to
 * print a mode-specific prompt. The muforth kernel lacks decent I/O
 * facilities. Until these are defined in startup.mu4, the prompts are
 * noops.
 */
CODE(mu_nope)
CODE(muboot_interpret_token)
CODE(muboot_compile_token)

static struct imode forth_interpreter =
    { XTK(muboot_interpret_token), XTK(mu_nope) };

static struct imode forth_compiler =
    { XTK(muboot_compile_token),   XTK(mu_nope) };

static CELL_T(struct imode *) state = CELL(&forth_interpreter);


void mu_consume()
{
    /* call the current consume function */
    execute_xtk(_(_(state)->eat));  
}

void mu_push_state()
{
    PUSH_ADDR(&state);
}

void mu_compiler_lbracket()
{
    _(state) = &forth_interpreter;
}

void mu_rbracket()
{
    _(state) = &forth_compiler;
}

#ifdef DEBUG_STACK
#define SHOW_STACK muboot_show_stack()
static void muboot_show_stack()
{
    int i;
    fprintf(stderr, "  --");
    for (i = 0; i < 4; i++)
        fprintf(stderr, "  %16llx", (uval)SP[3-i]);
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
        mu_consume();
        SHOW_STACK;
    }
    DROP(2);
}

/*
 * This will also be re-implemented in Forth, but with nice nesting of
 * various context variables: radix, first, start/end, etc.
 */
void muboot_load_file()    /* c-string-name */
{
    int fd;

    mu_open_file_ro();
    fd = TOP;
    mu_read_file();

    _(start) = (char *)ST1;
    _(end)   = (char *)ST1 + TOP;
    DROP(2);

    /* wait to reset these until just before we evaluate the new file */
    lineno = 1;
    _(first) = _(start);

    muboot_interpret();

    PUSH(fd);
    mu_close_file();
}
