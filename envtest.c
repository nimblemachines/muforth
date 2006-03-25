/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
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
 * This file is used by the build process to determine the cell size of the
 * machine it's running on, and what kind of division its CPU possesses. If
 * C hadn't been designed by brain-dead WEENIES, this kind of thing
 * wouldn't be necessary.
 */

#include <stdio.h>

int main(int argc, char *argv[])
{
    /* cell size */
    if (sizeof(int) == 4)
        printf("#define SH_CELL 2\n");
    else if (sizeof(int) == 8)
        printf("#define SH_CELL 3\n");
    else
        printf("#error \"What kind of machine are you running this on, anyway?\"\n");

    /* division type */
    if ((-7 / 4) == -2)
        printf("#define HOST_DIVIDE_FLOORS\n");

    /* verify that int and void * are the same size: */
    if (sizeof(int) != sizeof(void *))
        printf("#error \"Hmm. Pointer and int types are different sizes.\"\n");        
    return 0;
}
