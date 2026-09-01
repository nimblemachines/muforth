/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2026 David Frech. (Read the LICENSE for details.)
 */

/*
 * Include the 8x16 and 12x24 Spleen fonts from OpenBSD.
 */

#include "muforth.h"

/*
 * struct wsdisplay_font was copied from the OpenBSD source file
 * src/sys/dev/wscons/wsconsio.h
 */

struct wsdisplay_font {
#define	WSFONT_NAME_SIZE	32
	char name[WSFONT_NAME_SIZE];
	int index;
#define WSDISPLAY_MAXFONTCOUNT	8
	int firstchar, numchars;
	int encoding;
#define WSDISPLAY_FONTENC_ISO 0
#define WSDISPLAY_FONTENC_IBM 1
	u_int fontwidth, fontheight, stride;
#define WSDISPLAY_MAXFONTSZ	(512*1024)
	int bitorder, byteorder;
#define	WSDISPLAY_FONTORDER_KNOWN	0	/* i.e, no need to convert */
#define	WSDISPLAY_FONTORDER_L2R		1
#define	WSDISPLAY_FONTORDER_R2L		2
	void *cookie;
	void *data;
};

/*
 * spleen8x16.h and spleen12x24.h were copied from the OpenBSD source
 * directory src/sys/dev/wsfont/
 */

#include "spleen8x16.h"
#include "spleen12x24.h"

void mu_push_spleen_8x16()      { PUSH_ADDR(spleen8x16_data); }
void mu_push_spleen_12x24()     { PUSH_ADDR(spleen12x24_data); }
