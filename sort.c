/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2005 David Frech. All rights reserved, and all wrongs
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

/*
 * interface to C library's qsort (quicksort) function
 */

#include "muforth.h"
#include <stdlib.h>

#define C_IS_COMPLETELY_RETARDED    1

#ifdef  C_IS_COMPLETELY_RETARDED
static int struct_string_compare(const void *p1, const void *p2)
{
    const struct string *s1 = p1;
    const struct string *s2 = p2;

    return string_compare(s1->data, s1->length, s2->data, s2->length);
}
#else /* this should work, but doesn't */
static int struct_string_compare(const struct string *s1,
                                 const struct string *s2)
{
    return string_compare(s1->data, s1->length, s2->data, s2->length);
}
#endif

void mu_string_quicksort()
{
    /* base, number, size, compare */
    qsort((void *)SND, T, sizeof(struct string), struct_string_compare);
    DROP2;
}
