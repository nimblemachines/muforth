/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/*
 * FreeBSD (version 8 and later) support for USB devices.
 */

/*
 * FreeBSD's USB support was based directly on NetBSD's until FreeBSD 8.
 * This file supports only the newer FreeBSD USB stack.
 */

#if defined(__FreeBSD__) && (__FreeBSD__ >= 8)

#include "muforth.h"

#include <sys/types.h>
#include <dirent.h>         /* opendir, readdir */
#include <fcntl.h>          /* open */
#include <unistd.h>         /* close */
#include <sys/ioctl.h>      /* ioctl */
#include <dev/usb/usb.h>
#include <dev/usb/usb_ioctl.h>
#include <dev/usb/usbdi.h>

/*
 * On FreeBSD 8 (and later) there is a devfs, and USB devices show up as
 * they are enumerated. Unlike NetBSD and previous versions of FreeBSD, the
 * ugen devices now contain a _bus_ number, and only refer to endpoint
 * zero. And in any case they are simply symlinks to the _real_ devices,
 * which are now in /dev/usb/.
 *
 * So let's look there instead. Device names have the form /dev/usb/B.D.E,
 * where B is a bus number (starting from 0); D is a device number
 * (starting from 1); and E is an endpoint number, starting from 0.
 */

static int enumerate_devices(int idVendor, int idProduct)
{
    DIR *usbdir;
    struct dirent *dev;;

    usbdir = opendir("/dev/usb");

    /* Return no match if directory couldn't be opened. */
    if (usbdir == NULL) return 0;

    while ((dev = readdir(usbdir)) != NULL)
    {
        if (dev->d_name[0] == '.') continue;
        if (dev->d_name[dev->d_namlen - 1] == '0')
        {
#define PATHMAX 32
            char path[PATHMAX];
            int fd;
            int res;
            struct usb_device_descriptor dev_desc;

            /* Found endpoint 0 device; build path */
            concat_paths(path, PATHMAX, "/dev/usb", dev->d_name);

            fd = open(path, O_RDONLY);
            if (fd == -1) continue;
            res = ioctl(fd, USB_GET_DEVICE_DESC, &dev_desc);
            close(fd);
            if (res == -1) continue;

            if (UGETW(dev_desc.idVendor) == idVendor &&
                UGETW(dev_desc.idProduct) == idProduct)
            {
                int timeout = 5000; /* ms */
                fd = open(path, O_RDWR);    /* Re-open read-write */
                if (fd == -1) return -1;
                if (ioctl(fd, USB_SET_RX_TIMEOUT, &timeout) == -1)
                    return -1;
                if (ioctl(fd, USB_SET_TX_TIMEOUT, &timeout) == -1)
                    return -1;
                return fd;
            }
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
    ucr.ucr_data = (void *)UNHEAPIFY(ST1);
    ucr.ucr_addr = 0;
    ucr.ucr_flags = (req.bmRequestType == UT_READ_DEVICE)
                    ? USB_SHORT_XFER_OK : 0;
    fd = TOP;
    DROP(7);

    if (ioctl(fd, USB_DO_REQUEST, &ucr) == -1) return abort_strerror();
}

#endif
