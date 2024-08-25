/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
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

#ifdef DEBUG_USB_ENUMERATION
#include <stdio.h>
#endif

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

typedef int (*usb_match_fn)(char *, int vid, int pid);    /* filepath, vendor id, product id */
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
 *  <0 if error
 *   0 if directory exhausted
 *  >0 if match found
 */
static int foreach_dirent(char *path, path_ok_fn try_path,
                          usb_match_fn fn, int vid, int pid)
{
    char pathbuf[USB_PATH_MAX];
    DIR *pdir;
    struct dirent *pde;

    pdir = opendir(path);

    /* Return error if directory couldn't be opened. */
    if (pdir == NULL) return -1;

    while ((pde = readdir(pdir)) != NULL)
    {
        /* make sure this is a dirent we care about */
        if (try_path(pde))
        {
            int matched;

            concat_paths(pathbuf, USB_PATH_MAX, path, pde->d_name);
            matched = fn(pathbuf, vid, pid);

            /*
             * matched < 0 means matched but error opening read/write
             *        == 0 means no match
             *         > 0 means match and device opened read/write
             *
             * We want to keep looking if no match, but exit otherwise.
             */
            if (matched != 0)
            {
               closedir(pdir);
               return matched;   /* open fd of matched device or error */
            }
        }
    }
    closedir(pdir);
    return 0;   /* no match */
}

/*
 * match_device returns 0 if there was no match (or if it couldn't open a
 * device); >0 if it successfully matched the device and was able to open
 * it read/write; <0 if it matched a device but couldn't open it (the only
 * true error condition we care about).
 */
static int match_device(char *dev, int vid, int pid)
{
    int fd;
    struct usb_device_descriptor dev_desc;
    int count;

#ifdef DEBUG_USB_ENUMERATION
    fprintf(stderr, "match_device: trying %s\n", dev);
#endif
    fd = open(dev, O_RDONLY);
    if (fd == -1) return 0;

    count = read(fd, &dev_desc, USB_DT_DEVICE_SIZE);
    if (count != USB_DT_DEVICE_SIZE) return 0;
    /* was assert(count == USB_DT_DEVICE_SIZE, "couldn't read device descriptor"); */

    close(fd);

    if (__le16_to_cpu(dev_desc.idVendor) == vid &&
        __le16_to_cpu(dev_desc.idProduct) == pid)
        return open(dev, O_RDWR);   /* Re-open read-write; this could fail! */

    return 0;
}

static int is_bus_or_dev(struct dirent *pde)
{
    /* bus and device entry names are 3 decimal digits */
    return isdigit(pde->d_name[0]) ? -1 : 0;
}

static int enumerate_devices(char *bus, int vid, int pid)
{
    return foreach_dirent(bus, is_bus_or_dev, match_device, vid, pid);
}

/*
 * usb-find-device (vendor-id product-id -- dev -1 | 0)
 */
void mu_usb_find_device()
{
    int matched;
    int vid = ST1;
    int pid = TOP;

    /* Enumerate USB device tree, looking for a match */
    if (dir_exists(USB_ROOT1))
        matched = foreach_dirent(USB_ROOT1, is_bus_or_dev, enumerate_devices, vid, pid);
    else
        matched = foreach_dirent(USB_ROOT2, is_bus_or_dev, enumerate_devices, vid, pid);

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
    tr.data = (void *)UNHEAPIFY(ST1);
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
 * usb-read-pipe ( 'buffer size pipe# dev -- #read)
 */
void mu_usb_read_pipe()
{
    struct usbdevfs_bulktransfer tr;
    int fd;
    int nread;
    uint8_t ep = ST1;

    tr.ep = ep | 0x80;
    tr.len = ST2;
    tr.timeout = 4000 /* ms timeout */;
    tr.data = (void *)UNHEAPIFY(ST3);
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
 * usb-write-pipe ( 'buffer size pipe# dev)
 */
void mu_usb_write_pipe()
{
    struct usbdevfs_bulktransfer tr;
    int fd;
    uint8_t ep = ST1;

    tr.ep = ep & 0x7f;
    tr.len = ST2;
    tr.timeout = 4000 /* ms timeout */;
    tr.data = (void *)UNHEAPIFY(ST3);
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
 * We could enumerate all of /dev/, matching for "hidrawX", and trying each
 * of these to see if the vid & pid match, but we do something simpler: we
 * directly try /dev/hidraw0 to hidraw9, trying to match the vid and pid
 * with any that we can open.
 *
 * hid-read and hid-write are simply calls to read() and write() with the
 * arguments permuted.
 */

/*
 * match_hid returns 0 if there was no match (or if it couldn't open a
 * device); >0 if it successfully matched the device and was able to open
 * it read/write; <0 if it matched a device but couldn't open it (the only
 * true error condition we care about).
 */
static int match_hid(char *dev, int vid, int pid)
{
    int fd;
    struct hidraw_devinfo hid;

#ifdef DEBUG_USB_ENUMERATION
    fprintf(stderr, "match_hid: trying %s\n", dev);
#endif
    fd = open(dev, O_RDONLY);
    if (fd == -1) return 0;

    if (ioctl(fd, HIDIOCGRAWINFO, &hid) == -1)
        return 0;

    close(fd);

    /* XXX have vendor and product fields been rewritten from USB
     * endianness (little) to local host's? */

    /* Linux idiots! They defined these as _signed_ 16-bit ints. */
    if (hid.bustype == BUS_USB &&
        (unsigned)hid.vendor == vid && (unsigned)hid.product == pid)
        return open(dev, O_RDWR);   /* Re-open read-write; could fail! */

    return 0;
}

/*
 * hid-find-device (vendor-id product-id -- dev -1 | 0)
 */
void mu_hid_find_device()
{
    int matched;
    int devnum;
    char dev_hidraw[] = "/dev/hidrawX";
    int vid = ST1;
    int pid = TOP;

    /* Try hidraw0 to hidraw9 */
    for (devnum = '0'; devnum <= '9'; devnum++)
    {
        dev_hidraw[11] = devnum;
        matched = match_hid(dev_hidraw, vid, pid);
        if (matched != 0) break;
    }

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
    void *buffer = (void *) UNHEAPIFY(ST2);
    size_t len = ST1;
    int fd = TOP;       /* dev is just a file descriptor */

    DROP(2);
    TOP = read_carefully(fd, buffer, len);
}

/*
 * hid-write ( 'buffer size dev)
 */
void mu_hid_write()
{
    void *buffer = (void *) UNHEAPIFY(ST2);
    size_t len = ST1;
    int fd = TOP;       /* dev is just a file descriptor */

    DROP(3);
    write_carefully(fd, buffer, len);
}
