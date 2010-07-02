/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include "muforth.h"

#include <stdlib.h>     /* exit(3) */

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

