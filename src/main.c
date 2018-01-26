/*
 * This file is part of muforth: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"

#include <stdlib.h>     /* exit(3) */

static struct counted_string *pcmd_line;

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
        pline = string_copy(pline, *argv++);
        *pline++ = ' ';
    }
    pcmd_line->length = pline - pcmd_line->data;

    /*
     * No need to null-terminate! This string is evaluated by the Forth
     * parser, not C code. Any pieces - like filenames - that get passed to
     * C are copied out of this string into the dictionary and
     * null-terminated first - just like input from _any other_ source.
     */
    ph = (cell *)ALIGNED(pline);
}

void mu_push_command_line()
{
    PUSH_ADDR(&pcmd_line->data);
}

void mu_bye()
{
    exit(0);
}

int main(int argc, char *argv[])
{
    muforth_init();
    convert_command_line(argc, argv);
    muforth_start();
    return 0;
}
