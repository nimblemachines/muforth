/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* time routines for muforth */

#include "muforth.h"

#include <time.h>

/* time and date */
static void push_forth_time_from_libc_time (struct tm *ptm, char *tz)
{
    STK(-9) = strlen (tz);
    STK(-8) = (cell_t) tz;
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
    push_forth_time_from_libc_time (&tm, tzname[tm.tm_isdst]);
}

void mu_global_time()
{
    struct tm tm;
    time_t clock;

    clock = POP;
    gmtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm, "UTC");
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

