/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2012 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/*
 * OSX support for USB devices.
 */

#include "muforth.h"

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
