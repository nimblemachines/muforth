/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* access to PCI config space */

/* Because FreeBSD tries to keep us from shooting ourselves, we have to do
   this thru ioctls rather than simply reading from i/o space. Sigh. */

#ifdef __FreeBSD__
#include "muforth.h"

#include <sys/pciio.h>
#include <fcntl.h>

int fd_pci;

void mu_pci_read()
{
    struct pci_io p;

    p.pi_sel.pc_bus  = (TOP >> 16) & 0xff;
    p.pi_sel.pc_dev  = (TOP >> 11) & 0x1f;
    p.pi_sel.pc_func = (TOP >> 8)  & 0x07;
    p.pi_reg         =  TOP        & 0xff;
    p.pi_width       = 4;
  
    STK(-1) = ioctl(fd_pci, PCIOCREAD, &p);
    TOP = p.pi_data;
    DROP(-1);
}

/* We need to open R/W, otherwise the ioctl, above, fails. */
void mu_pci_open()
{
    fd_pci = open("/dev/pci", O_RDWR); /* XXX: doesn't catch any errors! */
}

#endif
