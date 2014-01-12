/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * NetBSD, DragonflyBSD, & FreeBSD (version 7 and earlier) support for USB
 * devices.
 */

/*
 * FreeBSD's USB support was based directly on NetBSD's until FreeBSD 8.
 * Since DragonFly forked from FreeBSD 4, DragonFly also inherited NetBSD's
 * USB stack. Thus this file supports NetBSD, DragonFly, and older FreeBSD
 * systems.
 */

#if defined(__NetBSD__) || defined(__DragonFly__) || (__FreeBSD__ < 8)

#include "muforth.h"

#include <fcntl.h>          /* open */
#include <unistd.h>         /* close */
#include <sys/ioctl.h>      /* ioctl */
#ifdef __DragonFly__
#include <bus/usb/usb.h>
#else
#include <dev/usb/usb.h>
#endif

/*
 * On NetBSD ugen devices are predefined - there is no devfs. On my system
 * there are 4 ugens (0 to 3) each with 16 endpoints - 00 to 15. Device
 * names are of the form "ugenD.EE".
 *
 * On DragonFly and FreeBSD 7 (and back to version 5) there is a devfs, and
 * ugen devices show up as devices are enumerated. The endpoint zero device
 * has the name "ugenD" (where D starts at 0); other endpoints show up as
 * "ugenD.E" for endpoints 1 to 9, and, presumably, as "ugenD.EE" for
 * endpoints 10 to 15.
 */

#ifdef __NetBSD__
#define UGEN_EP0    "/dev/ugen0.00"
#else
#define UGEN_EP0    "/dev/ugen0"
#endif
#define UGEN_DEVNUM 9       /* string offset to device number */

static int enumerate_devices(int idVendor, int idProduct)
{
    usb_device_descriptor_t dev_desc;
    int fd;
    int res;
    int devnum;
    char dev_ep0[] = UGEN_EP0;

    for (devnum = '0'; devnum < '4'; devnum++)
    {
        dev_ep0[UGEN_DEVNUM] = devnum;
        fd = open(dev_ep0, O_RDONLY);
        if (fd == -1) continue;
        res = ioctl(fd, USB_GET_DEVICE_DESC, &dev_desc);
        close(fd);
        if (res == -1) continue;

        if (UGETW(dev_desc.idVendor) == idVendor &&
            UGETW(dev_desc.idProduct) == idProduct)
        {
            int timeout = 5000; /* ms */
            fd = open(dev_ep0, O_RDWR);   /* Re-open read-write */
            if (fd == -1) return -1;
            if (ioctl(fd, USB_SET_TIMEOUT, &timeout) == -1)
                return -1;
            return fd;
        }
    }
    return 0;
}

/*
 * usb-find-device (vendor-id product-id -- handle -1 | 0)
 */
void mu_usb_find_device()
{
    int matched;

    /* Enumerate USB device tree, looking for a match */
    matched = enumerate_devices(ST1, TOP);

    if (matched < 0) return abort_strerror();

    if (matched == 0)
    {
        /* No match found */
        DROP(1);
        TOP = 0;
    }
    else
    {
        /* Matched; return the device's _open_ file descriptor */
        ST1 = matched;
        TOP = -1;
    }
}

/*
 * usb-close (handle)
 */
void mu_usb_close()
{
    mu_close_file();
}

/*
 * usb-control (bmRequestType bRequest wValue wIndex wLength 'buffer device - count)
 */
void mu_usb_control()
{
    struct usb_ctl_request ucr;
    int fd;

#define req ucr.ucr_request
    req.bmRequestType = SP[6];
    req.bRequest = SP[5];
    USETW(req.wValue, SP[4]);
    USETW(req.wIndex, ST3);
    USETW(req.wLength, ST2);
    ucr.ucr_data = (void *)ST1;
    ucr.ucr_addr = 0;
    ucr.ucr_flags = (req.bmRequestType == UT_READ_DEVICE)
                    ? USBD_SHORT_XFER_OK : 0;
    fd = TOP;
    DROP(6);

    if (ioctl(fd, USB_DO_REQUEST, &ucr) == -1)
    {
        TOP = 0;    /* count of bytes transferred */
        return abort_strerror();
    }
    TOP = ucr.ucr_actlen;   /* actual length transferred */
}

#endif
