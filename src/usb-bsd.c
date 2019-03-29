/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2019 David Frech. (Read the LICENSE for details.)
 */

/*
 * BSD support for USB devices.
 *
 * Things are complicated because everyone originally inherited NetBSD's
 * USB subsystem, but DragonFly and FreeBSD have diverged - mostly in how
 * devices are named. This makes our job more difficult, and the code
 * harder to read.
 *
 * There are basically three different versions of the BSD USB stack:
 *
 *   - NetBSD and OpenBSD (the original style)
 *   - DragonFlyBSD and FreeBSD up to version 8 ("old FreeBSD")
 *   - FreeBSD version 8 and beyand ("new FreeBSD")
 */

#if defined(__NetBSD__) || defined(__OpenBSD__)
#define MU_USB_NETBSD
#endif
#if defined(__DragonFly__) || (defined(__FreeBSD__) && (__FreeBSD__ < 8))
#define MU_USB_FREEBSD
#endif
#if (defined(__FreeBSD__) && (__FreeBSD__ >= 8))
#define MU_USB_FREEBSD_8
#endif


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
#ifdef MU_USB_FREEBSD_8
#include <dev/usb/usb_ioctl.h>
#include <dev/usb/usbdi.h>
#define USBD_SHORT_XFER_OK  USB_SHORT_XFER_OK
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
 *
 * On FreeBSD 8 and beyond, ugen devices are symlinks into /dev/usb/, and
 * the bus number becomes part of the name: ugenB.D links to
 * /dev/usb/B.D.0, where B is a bus number (starting at 0), and D is a
 * device number (starting at 1). The last part is an endpoint number
 * (starting at 0).
 *
 * This means that FreeBSD 8+ ugen devices look kinda similar to the other
 * BSDs, but the numbers means completely different things! Don't be
 * confused. ;-)
 */

#ifndef MU_USB_FREEBSD_8

#ifdef MU_USB_NETBSD
#define UGEN_EP0    "/dev/ugen%d.00"
#else
#define UGEN_EP0    "/dev/ugen%d"
#endif

struct mu_usb_dev
{
    uint32_t fd;
    uint8_t devnum;
    uint8_t padding[3];
};

static int enumerate_devices(int vid, int pid, struct mu_usb_dev *mud)
{
    struct usb_device_info udi;
    int fd;
    int res;
    int dev;
#define PATHMAX 32
    char path[PATHMAX];

    for (dev = 0; dev < 10; dev++)
    {
        snprintf(path, PATHMAX, UGEN_EP0, dev);
        fd = open(path, O_RDONLY);
        if (fd == -1) { continue; }
        res = ioctl(fd, USB_GET_DEVICEINFO, &udi);
        close(fd);
        if (res == -1) { continue; }

        if (udi.udi_vendorNo == vid &&
            udi.udi_productNo == pid)
        {
            int timeout = 5000; /* ms */
            fd = open(path, O_RDWR);   /* Re-open read-write */
            if (fd == -1) { return -1; }
            /* Try to set timeout, but don't give up if it fails. */
            ioctl(fd, USB_SET_TIMEOUT, &timeout);
            /* Fill in the device structure passed to us. */
            mud->fd = fd;
            mud->devnum = dev;
            return fd;
        }
    }
    return 0;
}

#else   /* FreeBSD 8+ */

struct mu_usb_dev
{
    uint32_t fd;
    uint8_t busnum;
    uint8_t devnum;
    uint8_t padding[2];
};

static int enumerate_devices(int vid, int pid, struct mu_usb_dev *mud)
{
    struct usb_device_info udi;
    int fd;
    int res;
    int bus;
    int dev;
#define PATHMAX 32
    char path[PATHMAX];

    for (bus = 0; bus < 8; bus++)
    {
        for (dev = 1; dev < 128; dev++)
        {
            snprintf(path, PATHMAX, "/dev/usb/%d.%d.0", bus, dev);
            fd = open(path, O_RDONLY);
            if (fd == -1) { break; }
            res = ioctl(fd, USB_GET_DEVICEINFO, &udi);
            close(fd);
            if (res == -1) { continue; }

            if (udi.udi_vendorNo == vid &&
                udi.udi_productNo == pid)
            {
                int timeout = 5000; /* ms */
                fd = open(path, O_RDWR);    /* Re-open read-write */
                if (fd == -1) { return -1; }
                if (ioctl(fd, USB_SET_RX_TIMEOUT, &timeout) == -1)
                {
                    fprintf(stderr, "couldn't set rx timeout for %s\n", path);
                }
                if (ioctl(fd, USB_SET_TX_TIMEOUT, &timeout) == -1)
                {
                    fprintf(stderr, "couldn't set tx timeout for %s\n", path);
                }
                /* Fill in the device structure passed to us. */
                mud->fd = fd;
                mud->busnum = bus;
                mud->devnum = dev;
                return fd;
            }
        }
    }
    return 0;
}

#endif  /* ifndef FreeBSD 8+ */

/*
 * usb-find-device (vendor-id product-id -- handle -1 | 0)
 */
void mu_usb_find_device()
{
    int matched;
    int vid = ST1;
    int pid = TOP;

    DROP(2);
    mu_here();      /* push current heap pointer */

    /* Enumerate USB device tree, looking for a match */
    matched = enumerate_devices(vid, pid, (struct mu_usb_dev *)TOP);

    /*
     * enumerate_devices only returns failure (-1) if it found a match but
     * couldn't open the device for read & write. Tell the user about the
     * error.
     */
    if (matched < 0) return abort_strerror();

    if (matched == 0)
    {
        /* No match found */
        TOP = 0;
    }
    else
    {
        /*
         * Matched; return a struct containing the matched device's _open_
         * file descriptor, and a device number (for everything but FreeBSD
         * 8+), or a bus number and device number (for FreeBSD 8+).
         */
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
    struct mu_usb_dev *mud = (struct mu_usb_dev *)ST1;
    int pipe_fd;
#define PATHMAX 32
    char path[PATHMAX];

#ifdef MU_USB_NETBSD
    snprintf(path, PATHMAX, "/dev/ugen%d.%.2ld", mud->devnum, TOP);
#else
#ifdef MU_USB_FREEBSD
    snprintf(path, PATHMAX, "/dev/ugen%d.%ld", mud->devnum, TOP);
#else   /* FREEBSD 8+ */
    snprintf(path, PATHMAX, "/dev/usb/%d.%d.%ld", mud->busnum, mud->devnum, TOP);
#endif
#endif
    pipe_fd = open(path, flags);
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
 * usb-control (bmRequestType bRequest wValue wIndex wLength 'buffer dev - count)
 */
void mu_usb_control()
{
    struct mu_usb_dev *mud = (struct mu_usb_dev *)TOP;
    struct usb_ctl_request ucr;

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
    DROP(6);

    if (ioctl(mud->fd, USB_DO_REQUEST, &ucr) == -1)
    {
        TOP = 0;    /* count of bytes transferred */
        return abort_strerror();
    }
    TOP = ucr.ucr_actlen;   /* actual length transferred */
}

/*
 * The ioctl USB_GET_DEVICINFO only seems to be defined for uhid(4) devices
 * on NetBSD and OpenBSD, so we are only going to define hid-find-device
 * for these two OSes. On DragonFly and FreeBSD we will assume that
 * /dev/uhid0 is the device we want!
 */
#ifdef MU_USB_NETBSD

#define UHID        "/dev/uhid%d"

static int enumerate_hid_devices(int vid, int pid)
{
    struct usb_device_info udi;
    int fd;
    int res;
    int dev;
#define PATHMAX 32
    char path[PATHMAX];

    for (dev = 0; dev < 10; dev++)
    {
        snprintf(path, PATHMAX, UHID, dev);
        fd = open(path, O_RDONLY);
        if (fd == -1) { continue; }
        res = ioctl(fd, USB_GET_DEVICEINFO, &udi);
        close(fd);
        if (res == -1) { continue; }

        if (udi.udi_vendorNo == vid &&
            udi.udi_productNo == pid)
        {
            return open(path, O_RDWR);   /* Re-open read-write */
        }
    }
    return 0;
}

/*
 * hid-find-device (vendor-id product-id -- handle -1 | 0)
 */
void mu_hid_find_device()
{
    int matched;

    /* Enumerate USB device tree, looking for a match */
    matched = enumerate_hid_devices(ST1, TOP);

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

#endif  /* MU_USB_NETBSD */
