/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
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

int main(int argc, char *argv[])
{
    /* Division type */
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

    return 0;
}
