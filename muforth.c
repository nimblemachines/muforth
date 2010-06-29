/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include "muforth.h"
#include "version.h"

#include <stdlib.h>

/* data stack */
cell stack[STACK_SIZE];
cell *SP;

/* return stack */
xtk *rstack[STACK_SIZE];
xtk **RP;

xtk   *IP;     /* instruction pointer */
xtk    W;      /* on entry, points to the current Forth word */

int  cmd_line_argc;
char **cmd_line_argv;

static void init_stacks()
{
    mu_sp_reset();
    RP = R0;
}

static void mu_find_init_file()
{
    /* These don't have to be proper counted strings because they are
     * used only by C code hereafter.
     */
    PUSH("startup.mu4");
    mu_readable_q();
    if (POP) return;

    PUSH("/usr/local/share/muforth/startup.mu4");
    mu_readable_q();
    if (POP) return;

    die("couldn't find startup.mu4 init file");
}

/*
  ( We need to rebuild the command line - says something about the
  inefficiency of C's way of doing cmd line params - and parse that.)

  ( copy with trailing blank, but don't include the blank in the new addr)
  : "copy  ( to from u - to+u)   push  over  r@ cmove  pop +  bl over c!  ;

  : command-line  ( skip first one: the program name)
  ram  argv cell+  ram  argc 1- ?for
  over @  dup string-length "copy  1+ ( keep blank)  cell u+  next  then
  nip  ( start end)  over -  ( a u)  dup allot ( aligns) ;
*/

static struct counted_string *pcmd_line;

static char *copy_string(char *to, char *from, size_t length)
{
    bcopy(from, to, length);
    to += length;
    *to++ = ' ';
    return to;
}

static void convert_command_line(int argc, char *argv[])
{
    char *pline;

    /* skip arg[0] */
    argc--;
    argv++;

    pcmd_line = (struct counted_string *)ph;
    pline = pcmd_line->data;

    while (argc--)
    {
        pline = copy_string(pline, *argv, strlen(*argv));
        argv++;
    }
    pcmd_line->length = pline - pcmd_line->data;

    /* null terminate and align */
    *pline++ = 0;
    ph = (cell *)ALIGNED(pline);
}

void mu_push_command_line()
{
    PUSH(&pcmd_line->data);
}

void mu_push_build_time()
{
    PUSH(build_time);
}

static void mu_start_up()
{
    PUSH("warm");       /* push the token "warm" */
    PUSH(4);
    muboot_interpret_token();    /* ... and execute it! */
}

void mu_bye()
{
    exit(0);
}

int main(int argc, char *argv[])
{
    init_dict();
    convert_command_line(argc, argv);
    init_stacks();
    mu_find_init_file();
    _mu_load_file();
    mu_start_up();
    return 0;
}

