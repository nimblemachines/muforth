/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* tty support for muForth, both for the console and attached targets */

#include "muforth.h"

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>         /* isatty */

/* stack: ( fd termios - sizeof(termios) ) */
void mu_get_termios()
{
    tcgetattr(ST1, (struct termios *)TOP);

    DROP(1);
    TOP = sizeof(struct termios);
}

/* stack: ( fd termios - ) */
void mu_set_termios()
{
    /* drain out, flush in, set */
    if (tcsetattr(ST1, TCSAFLUSH, (struct termios *)TOP) == -1)
        return abort_strerror();

    DROP(2);
}

/*
 * I need two "raw" modes: one for human interaction (char by char), and
 * one for interacting with target devices connected via serial port.
 *
 * For humans, I want the interrupt chars to work, and I want no timeout (a
 * read will wait forever).
 *
 * For targets, I want no interrupt chars, no modem signalling, no hardware
 * handshake, but I want RS232 breaks to be received and rendered as null
 * chars; also I want a 0.5s timeout, so that code that expects a char from
 * the target will "immediately" exit with an error if there isn't a
 * character waiting.
 */

static void set_termios_raw(struct termios *pti)
{
#ifdef __CYGWIN__
    /* Cygwin lacks cfmakeraw, so we do it "by hand". */
    pti->c_iflag &= ~(PARMRK | ISTRIP | INLCR | IGNCR |
                      ICRNL | IXON | IXOFF);
    pti->c_iflag |= IGNBRK;

/*    pti->c_oflag &= ~OPOST;  */

    pti->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    pti->c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    pti->c_cflag |= (CS8 | CLOCAL);
#else
    cfmakeraw(pti);
#endif
}

void mu_set_termios_user_raw()
{
    struct termios *pti = (struct termios *) TOP;
    set_termios_raw(pti);
    pti->c_oflag |= (OPOST);  /* set opost, so newlines become CR/LF */
    /* pti->c_lflag |= (ISIG); */   /* accept special chars and gen signals */
    pti->c_cc[VMIN] = 1;
    pti->c_cc[VTIME] = 0;
    DROP(1);
}

void mu_set_termios_target_raw()
{
    struct termios *pti = (struct termios *) TOP;
    set_termios_raw(pti);
    pti->c_cflag &= ~(CRTSCTS);   /* no handshaking */
    pti->c_cflag |= (CLOCAL);     /* no modem signalling */
    pti->c_cc[VMIN] = 0;          /* return even if no chars avail */
    pti->c_cc[VTIME] = 5;         /* timeout in decisecs */
    DROP(1);
}

/* stack: ( speed termios - ) */
void mu_set_termios_speed()
{
    struct termios *pti = (struct termios *) TOP;

#define BPS(x)  case x: ST1 = B ## x; break

    switch(ST1)
    {
        BPS(  4800);
        BPS(  9600);
        BPS( 19200);
        BPS( 38400);
        BPS( 57600);
        BPS(115200);
        BPS(230400);
    default:
        return abort_zmsg("Unsupported speed");
    }

#ifdef __CYGWIN__
    /* Cygwin lacks cfsetspeed, so do it by hand. */
    pti->c_ospeed = pti->c_ispeed = ST1;
#else
    cfsetspeed(pti, ST1);
#endif
    DROP(2);
}

/*
 * We need to be able to send RS232 BREAKs. In at least one case (HC908
 * bootloader) target hardware requires this - or at least it may make
 * debugging the connection easier. ;-)
 */
/* stack: ( fd) */
void mu_tty_send_break()
{
    tcsendbreak(TOP, 0);         /* most implementions ignore the duration */
    DROP(1);
}

void mu_tty_iflush()
{
    tcflush(TOP, TCIFLUSH);          /* drain output, discard input */
    DROP(1);
}

/*
 * Support for querying the column width of terminals, and staying updated
 * (via SIGWINCH) when they change.
 *
 * In case the user has redirected stdout to a file, use stderr as the file
 * descriptor to test.
 */
void mu_tty_width()
{
    int fd = TOP;
    struct winsize tty_size;

    if (!isatty(fd))
    {
        TOP = 80;   /* default width for file output */
        return;
    }

    if (ioctl(fd, TIOCGWINSZ, &tty_size) == -1)
        return abort_strerror();

    TOP = tty_size.ws_col;
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

