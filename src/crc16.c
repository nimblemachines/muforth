/*
 * This file is part of muforth: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"

/*
 * Calculate a 16-bit CRC using the CRC16-CCITT polynomial,
 *
 *    16    12    5
 *   x   + x   + x  + 1
 *
 * I found a really clever implementation of this in C here:
 *    https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/420688
 * posted by Thierry Hischier.
 *
 * I have modified a few names, a few types, and the whitespace. But the
 * core algorithm is his.
 *
 * Instead of doing a table lookup, he calculates the results directly by
 * shifting and xoring the CRC accumulator with itself. In order to change
 * the polynomial, two lines of code would have to change.
 */
uint16_t crc16(uint8_t *p, size_t len)
{
    uint16_t crc = 0xFFFF;

    for (; len > 0; len--)
    {
        uint16_t x;
        x = ((crc >> 8) ^ *p++) & 0xff;
        x ^= x >> 4;    /* feed back high 4 bits of x^12 */
        crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }

    return crc;
}

void mu_crc16_c(void)
{
    ST1 = crc16((uint8_t *)ST1, TOP);
    DROP(1);
}
