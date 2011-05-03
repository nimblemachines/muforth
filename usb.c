/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * Against my better judgment, I'm adding libusb-0.1 support to muFORTH. I
 * need to be able to talk to targets over USB, and because every OS
 * supports USB very differently, and because Linux (in particular) is such
 * a moving target and so badly documented, I figured I'd punt on "rolling
 * my own" support for USB into muFORTH. This could still change - in
 * particular for Darwin and the BSDs - if I find time (and motivation) in
 * the future to work on it.
 *
 * Hmm. libusb-0.1.12 doesn't even build on Darwin. <sigh>
 */

#include "muforth.h"
#include <usb.h>

/*
 * This is currently a crock, and doesn't tell us much, but it at least
 * signals that something went wrong.
 */
static void abort_usb_error(int error)
{
    return abort_zmsg("USB error");
}

/*
 * usb-find-device (vendor-id product-id -- handle -1 | vid 0)
 */
void mu_usb_find_device()
{
    struct usb_bus *pbus;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (pbus = usb_get_busses(); pbus != NULL; pbus = pbus->next)
    {
        struct usb_device *pdev;

        for (pdev = pbus->devices; pdev != NULL; pdev = pdev->next)
        {
            if (pdev->descriptor.idVendor == ST1 &&
                pdev->descriptor.idProduct == TOP)
            {
                /* Open the device & return a handle */
                ST1 = (addr)usb_open(pdev);
                TOP = -1;       /* found */
                return;
            }
        }
    }
    TOP = 0;    /* not found */
}

void mu_usb_close()
{
    int err = usb_close((struct usb_dev_handle *)TOP);
    if (err < 0) return abort_usb_error(err);
    DROP(1);
}

/*
 * usb-control (handle bmRequestType bRequest wValue wIndex 'buffer wLength timeout)
 */
void mu_usb_control()
{
    int err;

    usb_dev_handle *pdevh = (usb_dev_handle *)SP[7];
    int bmReqType = SP[6];
    int bReq = SP[5];
    int wValue = SP[4];
    int wIndex = ST3;
    char *pbuf = (char *)ST2;
    int wLength = ST1;
    int timeout = TOP;

    err = usb_control_msg(pdevh, bmReqType, bReq, wValue, wIndex,
                          pbuf, wLength, timeout);
    if (err < 0) return abort_usb_error(err);
    DROP(8);
}
