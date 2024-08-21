/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* time routines for muforth */

#include "muforth.h"

#include <time.h>
#include <errno.h>

/* time and date */
static void push_forth_time_from_libc_time (struct tm *ptm, char *tz)
{
    DROP(-8);
    TOP = strlen (tz);
    ST1 = HEAPIFY(tz);
    ST2 = ptm->tm_sec;
    ST3 = ptm->tm_min;
    SP[4] = ptm->tm_hour;
    SP[5] = ptm->tm_yday;     /* 0 to 365 */
    SP[6] = ptm->tm_mday;
    SP[7] = ptm->tm_mon;      /* 0 to 11 */
    SP[8] = ptm->tm_year + 1900;
}

void mu_local_time()
{
    struct tm tm;
    time_t clock;

    clock = TOP;
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

    clock = TOP;
    gmtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm, "UTC");
}

void mu_push_clock()
{
    PUSH(time(NULL));       /* seconds since UNIX epoch */
}

/*
 * We need a way to do short sleeps for talking to sensitive hardware
 * targets (like the Freescale HC908 series). I'm not getting good data
 * from the bootloader, and wondering if some short pauses would help.
 */
void mu_nanosleep()
{
    struct timespec ts;

    ts.tv_sec = ST1;
    ts.tv_nsec = TOP;

    while (nanosleep(&ts, &ts) == -1)
    {
        if (errno == EINTR) continue;
        return abort_strerror();
    }
    DROP(2);
}
