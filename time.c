/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2009 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* time routines for muforth */

#include "muforth.h"

#include <time.h>

/* time and date */
static void push_forth_time_from_libc_time (struct tm *ptm, char *tz)
{
    DUP;
    NIP(-8);
    TOP = strlen (tz);
    ST1 = (cell) tz;
    ST2 = ptm->tm_sec;
    ST3 = ptm->tm_min;
    SP[3] = ptm->tm_hour;
    SP[4] = ptm->tm_yday;     /* 0 to 365 */
    SP[5] = ptm->tm_mday;
    SP[6] = ptm->tm_mon;      /* 0 to 11 */
    SP[7] = ptm->tm_year + 1900;
}

void mu_local_time()
{
    struct tm tm;
    time_t clock;

    clock = POP;
#ifndef XXX__NetBSD__
    tzset();
#endif
    localtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm, tzname[tm.tm_isdst]);
}

void mu_utc()
{
    struct tm tm;
    time_t clock;

    clock = POP;
    gmtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm, "UTC");
}

void mu_push_clock()
{
    PUSH(time(NULL));       /* seconds since UNIX epoch */
}

