/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2009 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* tty support for muForth, both for the console and attached targets */

#include "muforth.h"

#include <sys/ioctl.h>
#include <termios.h>

/* XXX: Use cfmakeraw, cfsetispeed, cfsetospeed? */

/* stack: ( fd termios - sizeof(termios) ) */
void mu_get_termios()
{
    tcgetattr(ST1, (struct termios *)TOP);

    NIP(1);
    TOP = sizeof(struct termios);
}

/* stack: ( fd termios - ) */
void mu_set_termios()
{
    /* drain out, flush in, set */
    if (tcsetattr(ST1, TCSAFLUSH, (struct termios *)TOP) == -1)
    {
        throw_strerror();
    }
    DROP(2);
}

void mu_set_termios_raw()
{
    struct termios *pti = (struct termios *) TOP;

    pti->c_iflag &= ~(PARMRK | ISTRIP | INLCR | IGNCR |
                      ICRNL | IXON | IXOFF);
    pti->c_iflag |= IGNBRK;

/*    pti->c_oflag &= ~OPOST;  */

    pti->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    pti->c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    pti->c_cflag |= (CS8 | CLOCAL);

    DROP(1);
}

void mu_set_termios_min_time()
{
    struct termios *pti = (struct termios *) ST2;
    pti->c_cc[VMIN] = ST1;
    pti->c_cc[VTIME] = TOP;
    DROP(3);
}

/* stack: ( speed termios - ) */
void mu_set_termios_speed()
{
    struct termios *pti = (struct termios *) TOP;

#define BPS(x)  case x: ST1 = B ## x; break

    switch(ST1)
    {
        BPS(  9600);
        BPS( 19200);
        BPS( 38400);
        BPS( 57600);
        BPS(115200);
        BPS(230400);
    default:
        throw("Unsupported speed");
    }
#ifdef __linux__
    /* Linux needs more than a poke in the eye with a stick. Should we
     * do the same with BSD?
     */
    cfsetspeed(pti, ST1);
#else
    pti->c_ospeed = pti->c_ispeed = ST1;
#endif
    DROP(2);
}

#if 0
/* This is for testing - to see what libc considers raw mode. */
void mu_raw_termios()
{
    struct termios before;
    struct termios after;
    int i;

    ioctl(0, TIOCGETA, &before);
    ioctl(0, TIOCGETA, &after);
    cfmakeraw(&after);

    for (i = 0; i < 4; i++)
        STK(-i-1) = ((uint *)&before)[i] ^ ((uint *)&after)[i];
    DROP(-4);
}
#endif




