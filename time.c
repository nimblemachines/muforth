/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* time routines for muforth */

#include "muforth.h"

#include <time.h>

/* time and date */
static void push_forth_time_from_libc_time (struct tm *ptm)
{
    char *tm_zone;

    tm_zone = tzname[ptm->tm_isdst];

    STK(-9) = strlen (tm_zone);
    STK(-8) = (int) tm_zone;
    STK(-7) = ptm->tm_sec;
    STK(-6) = ptm->tm_min;
    STK(-5) = ptm->tm_hour;
    STK(-4) = ptm->tm_yday;		/* 0 to 365 */
    STK(-3) = ptm->tm_mday;
    STK(-2) = ptm->tm_mon; 		/* 0 to 11 */
    STK(-1) = ptm->tm_year + 1900;
    DROP(-9);
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
    push_forth_time_from_libc_time (&tm);
}

void mu_global_time()
{
    struct tm tm;
    time_t clock;

    clock = POP;
    gmtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm);
}

void mu_push_clock()
{
    PUSH(time(NULL));		/* seconds since UNIX epoch */
}

#if 0
/* copied from d4 */
/* for finding out file modification times */
struct d4_file_times
{
    time_t modified;
    time_t accessed;
    time_t status_changed;
};

int glue_file_times (const char *filename, struct d4_file_times *pftimes)
{
    struct stat buf;

    if (stat (filename, &buf) == -1)
	return -(errno);

    pftimes->accessed       = buf.st_atime;
    pftimes->modified       = buf.st_mtime;
    pftimes->status_changed = buf.st_ctime;
    return 0;
}
#endif

