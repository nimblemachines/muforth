/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
	TOP = (int) counted_strerror();
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



    
