/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2012 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * NetBSD & FreeBSD support for USB devices.
 */

/*
 * FreeBSD's USB support was based directly on NetBSD's until FreeBSD 8.
 * FreeBSD 8 and later use slightly different include files, devices, etc.
 *
 * So, we "indirect" the choice of which one gets compiled: the NetBSD code
 * (for NetBSD and older FreeBSD), or the FreeBSD code (for newer - version
 * 8 and later - FreeBSD).
 */
#if defined(__FreeBSD__) && (__FreeBSD__ >= 8)
#include "usb-freebsd.c"
#define USBD_SHORT_XFER_OK  USB_SHORT_XFER_OK
#else
#include "usb-netbsd.c"
#endif

/*
 * Code common to both implementations.
 */

/*
 * usb-close ( handle)
 */
void mu_usb_close()
{
    mu_close_file();
}

/*
 * usb-request (bmRequestType bRequest wValue wIndex wLength 'buffer device)
 * XXX should return actual length of transfer?
 */
void mu_usb_request()
{
    struct usb_ctl_request ucr;
    int fd;

#define req ucr.ucr_request
    req.bmRequestType = SP[6];
    req.bRequest = SP[5];
    USETW(req.wValue, SP[4]);
    USETW(req.wIndex, ST3);
    USETW(req.wLength, ST2);
    /* req.timeout = 4000 /* ms timeout */;
    ucr.ucr_data = (void *)ST1;
    ucr.ucr_addr = 0;
    ucr.ucr_flags = (req.bmRequestType == UT_READ_DEVICE)
                    ? USBD_SHORT_XFER_OK : 0;
    fd = TOP;
    DROP(7);

    if (ioctl(fd, USB_DO_REQUEST, &ucr) == -1) return abort_strerror();
}
