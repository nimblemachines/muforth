/*
 * This file is part of muforth: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)
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

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || (defined(__FreeBSD__) && (__FreeBSD__ < 8))

#include "muforth.h"

#include <fcntl.h>          /* open */
#include <unistd.h>         /* close */
#include <sys/ioctl.h>      /* ioctl */
#include <stdio.h>          /* snprintf */
#ifdef __DragonFly__
#include <bus/usb/usb.h>
#else
#include <dev/usb/usb.h>
#endif

/*
 * On NetBSD and OpenBSD, ugen devices are predefined - there is no devfs.
 * On my system there are 4 ugens (0 to 3) each with 16 endpoints - 00 to
 * 15. Device names are of the form "ugenD.EE".
 *
 * On DragonFly and FreeBSD 7 (and back to version 5) there is a devfs, and
 * ugen devices show up as devices are enumerated. The endpoint zero device
 * has the name "ugenD" (where D starts at 0); other endpoints show up as
 * "ugenD.E" for endpoints 1 to 9, and, presumably, as "ugenD.EE" for
 * endpoints 10 to 15.
 */

#if defined(__NetBSD__) || defined(__OpenBSD__)
#define UGEN_EP0    "/dev/ugen0.00"
#else
#define UGEN_EP0    "/dev/ugen0"
#endif
#define UGEN_DEVNUM 9       /* string offset to device number */
#define UGEN_MAXLEN 16

#define UHID        "/dev/uhid0"
#define UHID_DEVNUM 9

struct mu_usb_dev
{
    int fd;
    int devnum;
};

static int enumerate_devices(char *dev_ep0, int devoff, int vid, int pid)
{
    struct usb_device_info udi;
    int fd;
    int res;
    int devnum;

    for (devnum = '0'; devnum <= '9'; devnum++)
    {
        dev_ep0[devoff] = devnum;
        fd = open(dev_ep0, O_RDONLY);
        if (fd == -1) { continue; }
        res = ioctl(fd, USB_GET_DEVICEINFO, &udi);
        close(fd);
        if (res == -1) { continue; }

        if (udi.udi_vendorNo == vid &&
            udi.udi_productNo == pid)
        {
            int timeout = 5000; /* ms */
            fd = open(dev_ep0, O_RDWR);   /* Re-open read-write */
            if (fd == -1) { return -1; }
            /* Try to set timeout, but don't give up if it fails. */
            ioctl(fd, USB_SET_TIMEOUT, &timeout);
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
    char ugen[] = UGEN_EP0;
    int matched;

    /* Enumerate USB device tree, looking for a match */
    matched = enumerate_devices(ugen, UGEN_DEVNUM, ST1, TOP);

    /*
     * enumerate_devices only returns failure (-1) if it found a match but
     * couldn't open the device for read & write. Tell the user about the
     * error.
     */
    if (matched < 0) return abort_strerror();

    if (matched == 0)
    {
        /* No match found */
        DROP(1);
        TOP = 0;
    }
    else
    {
        /*
         * Matched; return a struct containing the matched device's _open_
         * file descriptor, and a number that identifies which ugen device
         * it is: 0 for ugen0, 9 for ugen9.
         */
        struct mu_usb_dev *d;

        DROP(2);
        mu_here();      /* push current heap pointer */
        d = (struct mu_usb_dev *)TOP;
        d->fd = matched;
        d->devnum = ugen[UGEN_DEVNUM] - '0';
        PUSH(sizeof(struct mu_usb_dev));
        mu_allot();
        PUSH(-1);
    }
}

/*
 * On BSD, endpoints are represented as separate device files. Given a dev -
 * representing the endpoint 0 device - and a pipe/endpoint number, open
 * the file representing that endpoint of the device dev, using the open()
 * flags passed to us.
 */
static void mu_usb_open_pipe(int flags)     /* ( dev pipe# - pipe) */
{
    struct mu_usb_dev *d = (struct mu_usb_dev *)ST1;
    int pipe_fd;
    char dev[UGEN_MAXLEN];

#if defined(__NetBSD__) || defined(__OpenBSD__)
    snprintf(dev, UGEN_MAXLEN, "/dev/ugen%d.%.2lld", d->devnum, TOP);
#else
    snprintf(dev, UGEN_MAXLEN, "/dev/ugen%d.%lld", d->devnum, TOP);
#endif
    pipe_fd = open(dev, flags);
    if (pipe_fd == -1) return abort_strerror();
    DROP(1);
    TOP = pipe_fd;
}

/*
 * usb-open-pipe-ro  ( dev pipe# - pipe)
 * usb-open-pipe-wo  ( dev pipe# - pipe)
 * usb-open-pipe-rw  ( dev pipe# - pipe)
 *
 * Call mu_usb_open_pipe with open() flags implied by the function name.
 */
void mu_usb_open_pipe_ro()  { mu_usb_open_pipe(O_RDONLY); }
void mu_usb_open_pipe_wo()  { mu_usb_open_pipe(O_WRONLY); }
void mu_usb_open_pipe_rw()  { mu_usb_open_pipe(O_RDWR); }

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

/*
 * usb-read-pipe and usb-write-pipe are simply calls to read() and write()
 * with the arguments permuted. On BSD, a pipe is represented by a file
 * descriptor.
 */

/*
 * usb-read-pipe ( 'buffer size pipe -- #read)
 */
void mu_usb_read_pipe()
{
    cell fd = TOP;      /* 'buffer size pipe -- pipe 'buffer size */
    TOP = ST1;
    ST1 = ST2;
    ST2 = fd;

    mu_read_carefully();
}

/*
 * usb-write-pipe ( 'buffer size pipe)
 */
void mu_usb_write_pipe()
{
    cell fd = TOP;      /* 'buffer size pipe -- pipe 'buffer size */
    TOP = ST1;
    ST1 = ST2;
    ST2 = fd;

    mu_write_carefully();
}

/*
 * hid-find-device (vendor-id product-id -- handle -1 | 0)
 */
void mu_hid_find_device()
{
    char uhid[] = UHID;
    int matched;

    /* Enumerate USB device tree, looking for a match */
    matched = enumerate_devices(uhid, UHID_DEVNUM, ST1, TOP);

    /*
     * enumerate_devices only returns failure (-1) if it found a match but
     * couldn't open the device for read & write. Tell the user about the
     * error.
     */
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
 * hid-read and hid-write are simply calls to read() and write() with the
 * arguments permuted.
 */

/*
 * hid-read ( 'buffer size dev -- #read)
 */
void mu_hid_read()
{
    cell fd = TOP;      /* 'buffer size dev -- dev 'buffer size */
    TOP = ST1;
    ST1 = ST2;
    ST2 = fd;

    mu_read_carefully();
}

/*
 * hid-write ( 'buffer size dev)
 */
void mu_hid_write()
{
    cell fd = TOP;      /* 'buffer size dev -- dev 'buffer size */
    TOP = ST1;
    ST1 = ST2;
    ST2 = fd;

    mu_write_carefully();
}

#endif
