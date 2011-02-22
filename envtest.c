/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
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
#include <stdint.h>
#include <setjmp.h>

int main(int argc, char *argv[])
{
    /* division type */
    {
        int quot = -7 / 4;
        int rem  = -7 % 4;
        div_t r = div(-7, 4);

        if (quot != r.quot || rem != r.rem)
            printf("#error \"Weird. div(3) has different semantics than / and %%.\"\n");

#ifdef TEST_LLDIV
        {
            lldiv_t llr = lldiv(-7LL, 4LL);
            if (llr.quot != r.quot || llr.rem != r.rem)
                printf("#error \"Weird. div(3) has different semantics than lldiv(3).\"\n");
        }
#endif

        if (quot == -2 && rem == 1)
            printf("#define DIVISION_FLOORS\n");
        else if (quot == -1 && rem == -3)
            printf("#define DIVISION_IS_SYMMETRIC\n");
        else
            printf("#error \"Wow. Division Is broken.\"\n");
    }

#ifdef TEST_CELL_SIZE
    if (sizeof(intptr_t) == 4)
    {
        printf("#define CELL_SHIFT 2\n");
        printf("#define CELL_BITS  32\n");
    }
    else
    {
        printf("#define CELL_SHIFT 3\n");
        printf("#define CELL_BITS  64\n");
    }
#endif

#ifdef TEST_ENDIANNESS
    {
        int mem;
        uint8_t *pb = (uint8_t *)&mem;
        pb[0] = 0x11;
        pb[1] = 0x22;
        pb[2] = 0x33;
        pb[3] = 0x44;
        if (mem == 0x11223344)
            printf("#define BIG_ENDIAN\n");
        if (mem == 0x44332211)
            printf("#define LITTLE_ENDIAN\n");
    }
#endif

#ifdef TEST_JMPBUF_SIZE
    printf("#define JMPBUF_CELLS %d\n", (int)(sizeof(jmp_buf) / sizeof(intptr_t)));
#endif

    return 0;
}
