/* time routines for nuforth */

#include "muforth.h"

#include <time.h>

/* time and date */
void static push_forth_time_from_libc_time (struct tm *ptm)
{
    STK(-9) = strlen (ptm->tm_zone);
    STK(-8) = (int) ptm->tm_zone;
    STK(-7) = ptm->tm_sec;
    STK(-6) = ptm->tm_min;
    STK(-5) = ptm->tm_hour;
    STK(-4) = ptm->tm_yday;		/* 0 to 365 */
    STK(-3) = ptm->tm_mday;
    STK(-2) = ptm->tm_mon; 		/* 0 to 11 */
    STK(-1) = ptm->tm_year + 1900;
    DROP(-9);
}

void local_time()
{
    struct tm tm;
    time_t clock;

    clock = POP;
    localtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm);
}

void global_time()
{
    struct tm tm;
    time_t clock;

    clock = POP;
    gmtime_r (&clock, &tm);
    push_forth_time_from_libc_time (&tm);
}

void push_clock()
{
    PUSH(time(NULL));		/* seconds since UNIX epoch */
}
