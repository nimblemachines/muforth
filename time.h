/* time routines for nuforth */

#include <time.h>

/* time and date */
struct d4_time
{
    int cb_zone;		/* length of time zone string */
    const char *time_zone;
    int sec;
    int min;
    int hour;
    int day_of_year;
    int day_of_month;
    int month;
    int year;
};

void static d4time_from_libc_time (struct d4_time *ptime, struct tm *ptm)
{
    ptime->time_zone	= ptm->tm_zone;
    ptime->cb_zone	= strlen (ptm->tm_zone);
    ptime->sec		= ptm->tm_sec;
    ptime->min		= ptm->tm_min;
    ptime->hour		= ptm->tm_hour;
    ptime->day_of_year  = ptm->tm_yday;		/* 0 to 365 */
    ptime->day_of_month = ptm->tm_mday;
    ptime->month	= ptm->tm_mon; 		/* 0 to 11 */
    ptime->year		= ptm->tm_year + 1900;
}

void glue_local_time (time_t seconds_since_epoch, struct d4_time *ptime)
{
    struct tm tm_local;		/* allocate locally else new libc clobbers
				 * stuff in the middle of dforth - it doesn't
				 * respect the `brk' that we do at the start */

    localtime_r (&seconds_since_epoch, &tm_local);
    d4time_from_libc_time (ptime, &tm_local);
}

void glue_global_time (time_t seconds_since_epoch, struct d4_time *ptime)
{
    struct tm tm_local;		/* allocate locally else new libc clobbers
				 * stuff in the middle of dforth - it doesn't
				 * respect the `brk' that we do at the start */

    gmtime_r (&seconds_since_epoch, &tm_local);
    d4time_from_libc_time (ptime, &tm_local);
}

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
