/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * This file is used by the build process to determine the cell size of the
 * machine it's running on, and what kind of division its CPU possesses. If
 * C hadn't been designed by brain-dead WEENIES, this kind of thing
 * wouldn't be necessary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

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
    {
        int quot = -7 / 4;
        int rem  = -7 % 4;
        div_t r = div(-7, 4);
        lldiv_t llr = lldiv(-7LL, 4LL);

        if (quot != r.quot || rem != r.rem)
            printf("#error \"Weird. div(3) has different semantics than / and %%.\"\n");

        if (llr.quot != r.quot || llr.rem != r.rem)
            printf("#error \"Weird. div(3) has different semantics than lldiv(3).\"\n");

        if (quot == -2 && rem == 1)
            printf("#define DIVIDE_FLOORS\n");
        else if (quot == -1 && rem == -3)
            printf("#define DIVIDE_IS_SYMMETRIC\n");
        else
            printf("#error \"Wow. Divide is broken.\"\n");
    }

    /* verify that int and void * are the same size: */
    if (sizeof(int) != sizeof(void *))
        printf("#error \"Hmm. Pointer and int types are different sizes: "
            "sizeof(int) = %d, sizeof(void *) = %d\"\n",
            (int) sizeof(int), (int) sizeof(void *));

    /* show size of jmpbuf */
    printf("#define JMPBUF_CELLS %d\n", (int)(sizeof(jmp_buf) / sizeof(int)));

    return 0;
}
