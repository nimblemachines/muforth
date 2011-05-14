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
 * usb-find-device (vendor-id product-id -- handle -1 | 0)
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
    if (ioService == 0)
    {
        DROP(1);
        TOP = 0;
        return;
    }

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
    ST1 = (addr)deviceInterface;
    TOP = -1;
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
 * usb-request (bmRequestType bRequest wValue wIndex wLength 'buffer device)
 */
void mu_usb_request()
{
    IOUSBDevRequest req;
    IOReturn ior;
    IOUSBDeviceInterface **dev = (IOUSBDeviceInterface **)TOP;

    req.bmRequestType = SP[6];
    req.bRequest = SP[5];
    req.wValue = SP[4];
    req.wIndex = ST3;
    req.wLength = ST2;
    req.pData = (void *)ST1;
    DROP(7);
    ior = (*dev)->DeviceRequest(dev, &req);
    if (ior != kIOReturnSuccess)
        return abort_zmsg("DeviceRequest failed");
}

#else  /* USE_LIBUSB */

#include <usb.h>

/*
 * usb-find-device (vendor-id product-id -- handle -1 | 0)
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
                TOP = -1;
                return;
            }
        }
    }
    DROP(1);
    TOP = 0;
}

/*
 * usb-close ( handle)
 */
void mu_usb_close()
{
    int err = usb_close((struct usb_dev_handle *)TOP);
    DROP(1);
    if (err < 0) return abort_strerror();
}

/*
 * usb-request (bmRequestType bRequest wValue wIndex wLength 'buffer device)
 */
void mu_usb_request()
{
    int err;

    usb_dev_handle *pdevh = (usb_dev_handle *)TOP;
    int bmRequestType = SP[6];
    int bRequest = SP[5];
    int wValue = SP[4];
    int wIndex = ST3;
    int wLength = ST2;
    char *pbuf = (char *)ST1;
    DROP(7);

    err = usb_control_msg(pdevh, bmRequestType, bRequest, wValue, wIndex,
                          pbuf, wLength, 4000 /* ms timeout */);
    if (err < 0) return abort_strerror();
}

#endif
