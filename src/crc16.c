/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2022 David Frech. (Read the LICENSE for details.)
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
static uint16_t crc16_byte(uint16_t crc, uint8_t b)
{
    uint16_t x;
    x = ((crc >> 8) ^ b) & 0xff;
    x ^= x >> 4;    /* feed back high 4 bits of x^12 */
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;

    return crc;
}

static uint16_t crc16_buf(uint16_t crc, uint8_t *p, size_t len)
{
    for (; len > 0; len--)
        crc = crc16_byte(crc, *p++);

    return crc;
}

void mu_c_crc16_byte(void)  /* c-crc16-byte  ( crc byte - crc' */
{
    ST1 = crc16_byte(ST1, TOP);
    DROP(1);
}

void mu_c_crc16_buf(void)   /* c-crc16-buf   ( crc buf n - crc' */
{
    ST2 = crc16_buf(ST2, (uint8_t *)ST1, TOP);
    DROP(2);
}
