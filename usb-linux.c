/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2013 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * Linux support for USB devices.
 */

#include "muforth.h"

#include <ctype.h>          /* isdigit */
#include <sys/types.h>
#include <dirent.h>         /* opendir, readdir */
#include <fcntl.h>          /* open */
#include <unistd.h>         /* close */
#include <sys/ioctl.h>      /* ioctl */

/* Normal USB devices */
#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>

/* USB HIDs */
#include <linux/hidraw.h>
#include <linux/input.h>

/*
 * The two places USB devices can live. /dev/bus/usb is the "newer" place,
 * so if it exists and is readable we search it, otherwise we search
 * /proc/bus/usb.
 *
 * NOTE: We only search one or the other!
 */
#define USB_ROOT1 "/dev/bus/usb"
#define USB_ROOT2 "/proc/bus/usb"

#define USB_PATH_MAX (strlen(USB_ROOT2)+16)

struct match
{
    int idVendor;
    int idProduct;
};

typedef int (*usb_match_fn)(char *, struct match *);    /* filepath, match_data */
typedef int (*path_ok_fn)(struct dirent *);

/*
 * Check if a directory exists and is readable.
 *
 * Returns
 *   -1  if path is a directory and is readable
 *    0  otherwise
 */
static int dir_exists(char *path)
{
    DIR *pdir;

    pdir = opendir(path);

    /* Return false if directory couldn't be opened. */
    if (pdir == NULL) return 0;

    /* Ok, just close it and return true. Enumerator will open it again. */
    closedir(pdir);
    return(-1);
}

/*
 * Returns
 *   0 if directory exhausted or error
 *  >0 if match found
 */
static int foreach_dirent(char *path, path_ok_fn try_path,
                          usb_match_fn fn, struct match *pmatch)
{
    char pathbuf[USB_PATH_MAX];
    DIR *pdir;
    struct dirent *pde;

    pdir = opendir(path);

    /* Return no match if directory couldn't be opened. */
    if (pdir == NULL) return 0;

    while ((pde = readdir(pdir)) != NULL)
    {
        /* make sure this is a dirent we care about */
        if (try_path(pde))
        {
            char *subpath;
            int matched;

            /* build path right-to-left */
            subpath = path_prefix(pde->d_name, pathbuf + USB_PATH_MAX, '\0', pathbuf);
            subpath = path_prefix(path, subpath, '/', pathbuf);
            matched = fn(subpath, pmatch);
            if (matched != 0)
            {
               closedir(pdir);
               return matched;   /* error or match */
            }
        }
    }
    closedir(pdir);
    return 0;   /* no match */
}

static int read_le16(__le16 *pw)
{
    __u8 *pb = (__u8 *)pw;
    return pb[0] + (pb[1] << 8);
}

static int match_device(char *dev, struct match *pmatch)
{
    int fd;
    struct usb_device_descriptor dev_desc;
    int count;

    fd = open(dev, O_RDONLY);
    if (fd == -1) return -1;

    count = read(fd, &dev_desc, USB_DT_DEVICE_SIZE);
    assert (count == USB_DT_DEVICE_SIZE, "couldn't read device descriptor");

    close(fd);

    if (read_le16(&dev_desc.idVendor) == pmatch->idVendor &&
        read_le16(&dev_desc.idProduct) == pmatch->idProduct)
        return open(dev, O_RDWR);   /* Re-open read-write */

    return 0;
}

static int is_bus_or_dev(struct dirent *pde)
{
    /* bus and device entry names are 3 decimal digits */
    return isdigit(pde->d_name[0]) ? -1 : 0;
}

static int enumerate_devices(char *bus, struct match *pmatch)
{
    return foreach_dirent(bus, is_bus_or_dev, match_device, pmatch);
}

/*
 * usb-find-device (vendor-id product-id -- dev -1 | 0)
 */
void mu_usb_find_device()
{
    struct match match;
    int matched;

    match.idVendor = ST1;
    match.idProduct = TOP;

    /* Enumerate USB device tree, looking for a match */
    if (dir_exists(USB_ROOT1))
        matched = foreach_dirent(USB_ROOT1, is_bus_or_dev, enumerate_devices, &match);
    else
        matched = foreach_dirent(USB_ROOT2, is_bus_or_dev, enumerate_devices, &match);

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
         * Found a match; matched has the device's _open_ file descriptor.
         */
        ST1 = matched;
        TOP = -1;
    }
}

/*
 * usb-claim-interface  ( interface dev)
 */
void mu_usb_claim_interface()
{
    int intf = ST1;
    if (ioctl(TOP, USBDEVFS_CLAIMINTERFACE, &intf) == -1)
        return abort_strerror();

    DROP(2);
}

/*
 * usb-release-interface  ( interface dev)
 */
void mu_usb_release_interface()
{
    int intf = ST1;
    if (ioctl(TOP, USBDEVFS_RELEASEINTERFACE, &intf) == -1)
        return abort_strerror();

    DROP(2);
}

/*
 * usb-close ( dev)
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
    struct usbdevfs_ctrltransfer tr;
    int fd;
    int count;

    tr.bRequestType = SP[6];       /* should be called bmRequestType */
    tr.bRequest = SP[5];
    tr.wValue = SP[4];
    tr.wIndex = ST3;
    tr.wLength = ST2;
    tr.timeout = 4000 /* ms timeout */;
    tr.data = (void *)ST1;
    fd = TOP;
    DROP(6);

    if ((count = ioctl(fd, USBDEVFS_CONTROL, &tr)) == -1)
    {
        TOP = 0;    /* count of bytes transferred */
        return abort_strerror();
    }
    TOP = count;
}

/*
 * usb-read ( 'buffer size pipe# dev -- #read)
 */
void mu_usb_read()
{
    struct usbdevfs_bulktransfer tr;
    int fd;
    int nread;
    uint8_t ep = ST1;

    tr.ep = ep | 0x80;
    tr.len = ST2;
    tr.timeout = 4000 /* ms timeout */;
    tr.data = (void *)ST3;
    fd = TOP;
    DROP(3);

    if ((nread = ioctl(fd, USBDEVFS_BULK, &tr)) == -1)
    {
        TOP = 0;    /* #read */
        return abort_strerror();
    }
    TOP = nread;
}

/*
 * usb-write ( 'buffer size pipe# dev)
 */
void mu_usb_write()
{
    struct usbdevfs_bulktransfer tr;
    int fd;
    uint8_t ep = ST1;

    tr.ep = ep & 0x7f;
    tr.len = ST2;
    tr.timeout = 4000 /* ms timeout */;
    tr.data = (void *)ST3;
    fd = TOP;
    DROP(4);

    if (ioctl(fd, USBDEVFS_BULK, &tr) == -1)
        return abort_strerror();
}

/*
 * USB HID support.
 *
 * Unfortunately, several of the devices we want to talk to - Microchip's
 * PICKit2/3 and Freescale's Freedom board (running the CMSIS-DAP debug
 * firmware) - enumerate as HIDs so that they can work under Windows
 * without special USB drivers.
 *
 * For Linux, this means special support. We can't simply match the vid &
 * pid and open the device as a normal USB device and expect everything to
 * work... In fact, we can't open the "raw" USB device because the HID
 * subsystem beat us to it.
 *
 * Since these debug devices are not true HIDs, we want to use the _hidraw_
 * devices. All we need is to be able to send and receive raw reports - the
 * command and data buffers used by the debug firmware.
 *
 * We enumerate /dev/, matching for "hidrawX", and try each of these to see
 * if the vid & pid match.
 *
 * hid-read and hid-write are simply calls to read() and write().
 */

static int match_hid(char *dev, struct match *pmatch)
{
    int fd;
    struct hidraw_devinfo hid;
    unsigned int vid, pid;

    fd = open(dev, O_RDONLY);
    if (fd == -1) return -1;

    if (ioctl(fd, HIDIOCGRAWINFO, &hid) == -1)
        return -1;

    close(fd);

    /* Linux idiots! Defined these as _signed_ 16-bit ints. */
    vid = (unsigned short)hid.vendor;
    pid = (unsigned short)hid.product;

    /* XXX have vendor and product fields been rewritten from USB
     * endianness (little) to local host's? */ 
    if (hid.bustype == BUS_USB &&
        vid == pmatch->idVendor && pid == pmatch->idProduct)
        return open(dev, O_RDWR);   /* Re-open read-write */

    return 0;
}

static int is_hidraw(struct dirent *pde)
{
    return ((memcmp(pde->d_name, "hidraw", 6) == 0) && isdigit(pde->d_name[6]))
        ? -1 : 0;
}

/*
 * hid-find-device (vendor-id product-id -- dev -1 | 0)
 */
void mu_hid_find_device()
{
    struct match match;
    int matched;

    match.idVendor = ST1;
    match.idProduct = TOP;

    /* Enumerate /dev, looking for hidraw* */
    matched = foreach_dirent("/dev", is_hidraw, match_hid, &match);

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
         * Found a match; matched has the device's _open_ file descriptor.
         * Return it.
         */
        ST1 = matched;
        TOP = -1;
    }
}

/*
 * More Linux idiocy! On read, if HID doesn't use numbered reports, read()
 * simply returns the report.
 *
 * But on _writes_, no matter what, the first byte of the buffer to write
 * is the report number - 0 if the HID doesn't use numbered reports! So we
 * have to rebuffer the write. Grrr.
 *
 * Hmm... Actually this may not be true. I tried it without the rebuffering
 * and it seemed to work!
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
