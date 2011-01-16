/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
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
    int length = strlen(src);
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

    /* null terminate and align */
    *pline++ = 0;
    ph = (cell *)ALIGNED(pline);
}

void mu_push_command_line()
{
    PUSH(&pcmd_line->data);
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

