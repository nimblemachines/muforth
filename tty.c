/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* tty support for muForth, both for the console and attached targets */

#include "muforth.h"

#include <sys/ioctl.h>
#include <termios.h>

/* XXX: for testing only? */
#include <errno.h>

/* XXX: Use cfmakeraw, cfsetispeed, cfsetospeed? */

void mu_get_termios()
{
    tcgetattr(STK(1), (struct termios *)TOP);

    STK(1) = sizeof(struct termios);
    DROP(1);
}

void mu_set_termios()
{
    /* drain out, flush in, set */
    if (tcsetattr(STK(1), TCSAFLUSH, (struct termios *)TOP) == -1)
    {
	TOP = (int) strerror(errno);
	mu_throw();
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
    struct termios *pti = (struct termios *) STK(2);
    pti->c_cc[VMIN] = STK(1);
    pti->c_cc[VTIME] = TOP;
    DROP(3);
}

void mu_set_termios_speed()
{
    struct termios *pti = (struct termios *) STK(1);

#define BPS(x)	case x: TOP = B ## x; break

    switch(TOP)
    {
	BPS(  9600);
	BPS( 19200);
	BPS( 38400);
	BPS( 57600);
	BPS(115200);
	BPS(230400);
    default:
	TOP = (int) "Unsupported speed";
	mu_throw();
    }
    pti->c_ospeed = pti->c_ispeed = TOP;
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



    
