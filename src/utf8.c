/*
 * This file is part of muforth: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)
 */

/* Simple and ugly conversion from UTF-8 to wchar. */

#include "muforth.h"

#include <stdlib.h>
#include <xlocale.h>

static locale_t utf8_locale;

/*
 * utf8@+  ( a - wchar a+n)
 */
void mu_utf8_fetch_plus(void)
{
    int len;
    wchar_t wc;
    char *start = (char *)TOP;

    if (!utf8_locale)
        utf8_locale = newlocale(LC_CTYPE_MASK, "UTF-8", NULL);

    len = mbtowc_l(&wc, start, 4, utf8_locale);
    if (len < 0)
        return abort_strerror();

    if (len == 0)
    {
        /* Found a multibyte NULL */
        TOP = 0;
        PUSH_ADDR(start + 1);   /* skip the null! */
        return;
    }
    /*
     * Successfully converted a character. Push it and increment the
     * pointer.
     */
    TOP = wc;
    PUSH_ADDR(start + len);
}
