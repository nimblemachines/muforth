/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
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
static uint16_t crc16_byte_thierry(uint16_t crc, uint8_t b)
{
    uint16_t x;
    x = ((crc >> 8) ^ b) & 0xff;
    x ^= x >> 4;    /* feed back high 4 bits of x^12 */
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;

    return crc;
}

/*
 * This is the "reference" bitwise implementation from the X/YMODEM
 * protocol reference (ymodem.txt), edited by Chuck Forsberg.
 */
static uint16_t crc16_byte_xmodem(uint16_t crc, uint8_t b)
{
    int i;

    crc = crc ^ (b << 8);
    for (i = 0; i < 8; ++i)
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc = (crc << 1);

    return crc;
}

/*
 * crc16_byte points either to Thierry's implementation or to the X/YMODEM
 * reference implementation. We default to Thierry's.
 */
static uint16_t (*crc16_byte)(uint16_t, uint8_t) = crc16_byte_thierry;

/* Set the byte-wise CRC function. */
void mu_c_crc16_thierry(void) { crc16_byte = crc16_byte_thierry; }
void mu_c_crc16_xmodem(void)  { crc16_byte = crc16_byte_xmodem; }

void mu_c_crc16_byte(void)  /* c-crc16-byte  ( crc byte - crc') */
{
    ST1 = (*crc16_byte)(ST1, TOP);
    DROP(1);
}

static uint16_t crc16_buf(uint16_t crc, uint8_t *p, size_t len)
{
    for (; len > 0; len--)
        crc = (*crc16_byte)(crc, *p++);

    return crc;
}

void mu_c_crc16_buf(void)   /* c-crc16-buf  ( crc buf n - crc') */
{
    ST2 = crc16_buf(ST2, (uint8_t *)ST1, TOP);
    DROP(2);
}
