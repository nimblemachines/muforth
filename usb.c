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

#ifdef USE_IOKIT

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

/*
 * usb-find-device (vendor-id product-id -- handle)
 */
void mu_usb_find_device()
{
    CFMutableDictionaryRef matching;
    CFNumberRef numberRef;
    io_service_t ioService;
    IOCFPlugInInterface **pluginInterface;
    IOUSBDeviceInterface **deviceInterface;
    IOReturn ior;
    SInt32 score;      /* unused */
    SInt32 idVendor = ST1;
    SInt32 idProduct = TOP;

    /* Match instances of IOUSBDevice and its subclasses */
    matching = IOServiceMatching(kIOUSBDeviceClassName);
    if (matching == NULL) return abort_zmsg("IOServiceMatching returned NULL");

    /* Create a CFNumber for the idVendor and set the value in the dictionary */
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idVendor);
    CFDictionarySetValue(matching, CFSTR(kUSBVendorID), numberRef);
    CFRelease(numberRef);

    /* Create a CFNumber for the idProduct and set the value in the dictionary */
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idProduct);
    CFDictionarySetValue(matching, CFSTR(kUSBProductID), numberRef);
    CFRelease(numberRef);

    /* Look up our service. We have to release it when we're done. */
    ioService = IOServiceGetMatchingService(kIOMasterPortDefault, matching);
    if (ioService == 0) return abort_zmsg("No matching device found");

    ior = IOCreatePlugInInterfaceForService(ioService,
            kIOUSBDeviceUserClientTypeID,
            kIOCFPlugInInterfaceID,
            &pluginInterface,
            &score);

    if ((ior != kIOReturnSuccess) || (pluginInterface == NULL))
        return abort_zmsg("IOCreatePlugInInterfaceForService failed");

    /* Use the plugin interface to retrieve the device interface. */
    ior = (*pluginInterface)->QueryInterface(pluginInterface,
            CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
            (LPVOID*) &deviceInterface);

    /* We're done with the plugin interface. */
    (*pluginInterface)->Release(pluginInterface);

    if ((ior != kIOReturnSuccess) || (deviceInterface == NULL))
        return abort_zmsg("QueryInterface failed");

    /* Return deviceInterface as a handle for further operations on device. */
    DROP(1);
    TOP = (addr)deviceInterface;
}

/*
 * usb-close (handle)
 */
void mu_usb_close()
{
    IOUSBDeviceInterface **deviceInterface = (IOUSBDeviceInterface **)TOP;

    /* We're done with the device interface. */
    (*deviceInterface)->Release(deviceInterface);
    DROP(1);
}

/*
 * usb-control (handle bmRequestType bRequest wValue wIndex 'buffer wLength)
 */
void mu_usb_control()
{
    IOUSBDevRequest req;
    IOReturn ior;
    IOUSBDeviceInterface **dev = (IOUSBDeviceInterface **)SP[6];

    req.bmRequestType = SP[5];
    req.bRequest = SP[4];
    req.wValue = ST3;
    req.wIndex = ST2;
    req.pData = (void *)ST1;
    req.wLength = TOP;
    DROP(7);
    ior = (*dev)->DeviceRequest(dev, &req);
    if (ior != kIOReturnSuccess)
        return abort_zmsg("DeviceRequest failed");
}

#else  /* USE_LIBUSB */

#include <usb.h>

/*
 * This is currently a crock, and doesn't tell us much, but it at least
 * signals that something went wrong.
 */
static void abort_usb_error(int error)
{
    return abort_zmsg("libusb error");
}

/*
 * usb-find-device (vendor-id product-id -- handle)
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
                DROP(1);
                TOP = (addr)usb_open(pdev);
                return;
            }
        }
    }
    abort_zmsg("No matching device found");
}

/*
 * usb-close ( handle)
 */
void mu_usb_close()
{
    int err = usb_close((struct usb_dev_handle *)TOP);
    DROP(1);
    if (err < 0) return abort_usb_error(err);
}

/*
 * usb-control (handle bmRequestType bRequest wValue wIndex 'buffer wLength)
 */
void mu_usb_control()
{
    int err;

    usb_dev_handle *pdevh = (usb_dev_handle *)SP[6];
    int bmRequestType = SP[5];
    int bRequest = SP[4];
    int wValue = ST3;
    int wIndex = ST2;
    char *pbuf = (char *)ST1;
    int wLength = TOP;
    DROP(7);

    err = usb_control_msg(pdevh, bmRequestType, bRequest, wValue, wIndex,
                          pbuf, wLength, 4000 /* ms timeout */);
    if (err < 0) return abort_usb_error(err);
}

#endif
