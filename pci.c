/* access to PCI config space */

/* Because FreeBSD tries to keep us from shooting ourselves, we have to do
   this thru ioctls rather than simply reading from i/o space. Sigh. */

#ifdef __FreeBSD__
#include "muforth.h"

#include <sys/pciio.h>
#include <fcntl.h>

int fd_pci;

void pci_read()
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
void pci_open()
{
    fd_pci = open("/dev/pci", O_RDWR); /* XXX: doesn't catch any errors! */
}

#endif
