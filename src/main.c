/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2016 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"

#include <stdlib.h>     /* exit(3) */

static struct counted_string *pcmd_line;

/*
 * Unlike memcpy, which STOOPIDLY returns dest, rather than something
 * USEFUL, this routine returns dest + length. It also calculates the
 * length of the string for you - but doesn't copy the trailing null,
 * unlike strcpy.
 */
static char *str_copy(char *dest, char *src)
{
    size_t length = strlen(src);
    return memcpy(dest, src, length) + length;
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
        pline = str_copy(pline, *argv++);
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
