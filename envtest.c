/*
 * This file is part of muFORTH: http://sites.nimblemachines.com/muforth
 *
 * Copyright (c) 2002-2009 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
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
