/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2016 David Frech. (Read the LICENSE for details.)
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

        {
            lldiv_t llr = lldiv(-7LL, 4LL);
            if (llr.quot != r.quot || llr.rem != r.rem)
                printf("#error \"Weird. div(3) has different semantics than lldiv(3).\"\n");
        }

        if (quot == -2 && rem == 1)
            printf("#define MU_DIVISION_FLOORS\n");
        else if (quot == -1 && rem == -3)
            printf("#define MU_DIVISION_IS_SYMMETRIC\n");
        else
            printf("#error \"Wow. Division is broken.\"\n");
    }

    if (sizeof(intptr_t) == 4)
        printf("#define MU_ADDR_32\n");
    else if (sizeof(intptr_t) == 8)
        printf("#define MU_ADDR_64\n");
    else
        printf("#error. What kind of weird machine is this, anyway?\n");

    {
        int mem;
        uint8_t *pb = (uint8_t *)&mem;
        pb[0] = 0x11;
        pb[1] = 0x22;
        pb[2] = 0x33;
        pb[3] = 0x44;
        if (mem == 0x11223344)
            printf("#define MU_BIG_ENDIAN\n");
        if (mem == 0x44332211)
            printf("#define MU_LITTLE_ENDIAN\n");
    }

    printf("#define MU_JMPBUF_CELLS %d\n", (int)(sizeof(jmp_buf) / sizeof(intptr_t)));

    return 0;
}
