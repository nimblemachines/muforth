/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

#include "muforth.h"

#include <sys/mman.h>
#include <stdio.h>
#include <ctype.h>

/* data stack */
int stack[STACK_SIZE];
int *sp = S0;

int  cmd_line_argc;
char **cmd_line_argv;

u_int8_t *pnm0, *pcd0, *pdt0;	   /* ptrs to name, code, & data spaces */
u_int8_t *pnm, *pcd, *pdt;	/* ptrs to next free byte in each space */


void read_init_file()
{
    PUSH("startup.mu4");
    load_file();
}

void allocate()
{
    pnm0 = (char *) mmap(0, 256 * 4096, PROT_READ | PROT_WRITE,
			  MAP_ANON | MAP_PRIVATE, -1, 0);
    pcd0 = (char *) mmap(0, 256 * 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
			  MAP_ANON | MAP_PRIVATE, -1, 0);
    pdt0 = (char *) mmap(0, 1024 * 4096, PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
    if (pnm0 == MAP_FAILED || pcd0 == MAP_FAILED || pdt0 == MAP_FAILED)
	die("couldn't allocate memory");

    /* init compiler ptrs */
    pnm = pnm0;
    pcd = pcd0;
    pdt = pdt0;
}

int main(int argc, char *argv[])
{
    cmd_line_argc = argc;	/* copy to globals */
    cmd_line_argv = argv;

    allocate();
    init_dict();
    read_init_file();
    start_up();
    return 0;
}

