/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2005 David Frech. All rights reserved, and all wrongs
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

/* dictionary */

#include "muforth.h"
#include <stdio.h>

struct dict_entry
{
    /* char name[];           */
    /* char padding[];        */
    /* unsigned char length;  */
    struct dict_entry *link;  /* links point to link field */
    pw code;
    uint8 body[];
};

/* the forth and compiler "vocabulary" chains */
static struct dict_entry *forth_chain = NULL;
static struct dict_entry *compiler_chain = NULL;

/* current chain to compile into */
static struct dict_entry **current_chain = &forth_chain;

static xtk mu_name_hook = &p_mu_nope;  /* called when a name is created */

/* bogus C-style dictionary init */
struct inm          /* "initial name" */
{
    char *name;
    pw   code;
};

struct inm initial_forth[] = {
    { "version", mu_push_version },
    { "build-time", mu_push_build_time },
    { ":", mu_colon },
    { "<:>", mu_set_colon_code },
    { "name", mu_make_new_name },
    { "'name-hook", mu_push_tick_name_hook },
    { ".forth.", mu_push_forth_chain },
    { ".compiler.", mu_push_compiler_chain },
    { "current", mu_push_current },
    { "'number", mu_push_tick_number },
    { "'number,", mu_push_tick_number_comma },
    { "h", mu_push_h },
    { "r", mu_push_r },
    { "s0", mu_push_s0 },
    { "sp", mu_push_sp },
    { "catch", mu_catch },
    { "throw", mu_throw },
    { "command-line", mu_push_command_line },
    { "token", mu_token },
    { "parse", mu_parse },
    { "find", mu_find },
    { "execute", mu_execute },
    { "interpret", mu_interpret },
    { "evaluate", mu_evaluate },
    { "create-file", mu_create_file },
    { "open-file", mu_open_file },
    { "r/o", mu_push_ro_flags },
    { "r/w", mu_push_rw_flags },
    { "close-file", mu_close_file },
    { "mmap-file", mu_mmap_file },    
    { "load-file", mu_load_file },
    { "readable?", mu_readable_q },
    { "compile,", mu_compile },
    { "state", mu_push_state },
    { "-]", mu_minus_rbracket },
    { "parsed", mu_push_parsed },
    { "huh?", mu_huh },
    { "complain", mu_complain },
    { "^", mu_exit },
    { "<does>", mu_set_does_code },
    { "(branch)", mu_branch_ },
    { "(=0branch)", mu_eqzbranch_ },
    { "(0branch)", mu_zbranch_ },
    { "(next)", mu_next_ },
    { "(lit)", mu_literal_ },
    { "scrabble", mu_scrabble },
    { "nope", mu_nope },
    { "zzz", mu_zzz },
    { "+", mu_add },
    { "and", mu_and },
    { "or", mu_or },
    { "xor", mu_xor },
    { "negate", mu_negate },
    { "invert", mu_invert },
    { "u<", mu_uless },
    { "0<", mu_zless },
    { "0=", mu_zequal },
    { "<", mu_less },
    { "2*", mu_2star },
    { "2/", mu_2slash },
    { "u2/", mu_u2slash },
    { "<<", mu_shift_left },
    { ">>", mu_shift_right },
    { "u>>", mu_ushift_right },
    { "d+", mu_dplus },
    { "dnegate", mu_dnegate },
    { "um*", mu_um_star },
    { "m*", mu_m_star },
    { "um/mod", mu_um_slash_mod },
    { "fm/mod", mu_fm_slash_mod },
    { "@", mu_fetch },
    { "c@", mu_cfetch },
    { "!", mu_store },
    { "c!", mu_cstore },
    { "+!", mu_plus_store },
    { "rot", mu_rot },
    { "-rot", mu_minus_rot },
    { "dup", mu_dup },
    { "drop", mu_drop },
    { "2drop", mu_2drop },
    { "nip", mu_nip },
    { "swap", mu_swap },
    { "over", mu_over },
    { "push", mu_push },
    { "pop", mu_pop },
    { "r@", mu_rfetch },
    { "string-compare", mu_string_compare },
    { "read", mu_read_carefully },
    { "write", mu_write_carefully },
    { "sp@", mu_sp_fetch },
    { "sp!", mu_sp_store },
    { "cmove", mu_cmove },
    { "cells", mu_cells },
    { "cell/", mu_cell_slash },
 
    /* time.c */
    { "local-time", mu_local_time },
    { "utc", mu_global_time },
    { "clock", mu_push_clock },

#ifdef __FreeBSD__
    /* pci.c */
    { "pci", mu_pci_open },
    { "pci@", mu_pci_read },
#endif

    /* tty.c */
    { "get-termios", mu_get_termios },
    { "set-termios", mu_set_termios },
    { "set-raw", mu_set_termios_raw },
    { "set-min-time", mu_set_termios_min_time },
    { "set-speed", mu_set_termios_speed },

    /* select.c */
    { "fd-zero", mu_fd_zero },
    { "fd-set", mu_fd_set },
    { "fd-clr", mu_fd_clr },
    { "fd-in-set?", mu_fd_isset },
    { "select", mu_select },

    /* sort.c */
    { "string-quicksort", mu_string_quicksort },

    { "bye", mu_bye },

    { "#code", mu_push_code_size},
    { "#data", mu_push_data_size},
    { NULL, NULL }
};

struct inm initial_compiler[] = {
    { ";", mu_semicolon },
    { "[", mu_lbracket },
    { NULL, NULL }
};

void mu_push_current()
{
    PUSH(&current_chain);
}

void mu_push_forth_chain()
{
    PUSH(&forth_chain);
}

void mu_push_compiler_chain()
{
    PUSH(&compiler_chain);
}

/* 2004-apr-01. After giving a talk on muforth to SVFIG, and in particular
 * after Randy asked me some pointed questions, I decided that find should
 * have positive logic after all. I have renamed -"find to find to indicate
 * this change.
 */
/* find  ( a u chain - a u 0 | code -1) */
void mu_find()
{
    char *token = (char *) TRD;
    cell length = SND;
    struct dict_entry *pde = (struct dict_entry *) T;
    char *pnm;
    cell dict_length;

    while ((pde = pde->link) != NULL)
    {
        pnm = (uint8 *)&pde->link;
        dict_length = pnm[-1];
        if (dict_length != length) continue;
        if (memcmp(pnm - ALIGNED(dict_length + 1), token, length) != 0)
            continue;

        /* found: drop token, push code address and true flag */
        NIP;
        SND = (cell) &pde->code;  /* XXX: should be body? */
        T = -1;
        return;
    }
    /* not found: leave token, push false */
    T = 0;
}

static void compile_dict_entry(
    struct dict_entry **ppde, char *name, int length)
{
    char *pnm;
    struct dict_entry *pde;

    pnm = (char *)ALIGNED(pcd);

    /* Lay down the name and padding. Leave room for count byte. */
    memcpy(pnm, name, length);                 /* copy name string */
    pnm = (char *)ALIGNED(pnm + length + 1);   /* "allot" it, with count */
    pnm[-1] = length;                          /* store count */

    pde = (struct dict_entry *)pnm;
    pde->link = *ppde;      /* compile link to last */
    *ppde = pde;        /* link onto front of chain */

    /* Allot entry; now pointing to code field. */
    pcd = (cell *)&pde->code;

#if defined(BEING_DEFINED)
    printf("  %p %.*s\n", pcd, length, name);
#endif
}

/* Called from Forth. Only creates a name; does NOT set the code field! */
void mu_compile_name()
{
    compile_dict_entry(current_chain, (char *)SND, T);
    DROP2;
}

void mu_make_new_name()
{
    mu_token();
    EXEC(mu_name_hook);
    mu_compile_name();
}

void mu_push_tick_name_hook()
{
    PUSH(&mu_name_hook);
}

static void init_chain(struct dict_entry **pchain, struct inm *pinm)
{
    for (; pinm->name != NULL; pinm++)
    {
        compile_dict_entry(pchain, pinm->name, strlen(pinm->name));
        *pcd++ = (cell)pinm->code;  /* set code pointer */
        }
}

void init_dict()
{
    init_chain(&forth_chain, initial_forth);
    init_chain(&compiler_chain, initial_compiler);
}

static void print_name(cell *pcode)
{
    char *pnm = (char *)(pcode - 1);  /* back up over link field */
    int length = pnm[-1];

    printf("%.*s", length, pnm - ALIGNED(length + 1));
}


/* returns termination flag */
static int print_w(xtk *ip)
{
    xtk w = *ip;

    if ((w > (xtk)pcd0) && (w < (xtk)pcd) && (*w < (pw)pcd0))
    {
        printf("\n%p: ", ip);
        print_name((cell *)w);
    }
    else
    {
        printf(" %x", (cell)w);
    }
    return w == &p_mu_exit;
}

void dis(xtk *ip)
{
    if (*(pw *)ip == &mu_do_colon)
        print_name((cell *)ip++);

    for (;;)
    {
        if (print_w(ip++)) break;
    }
    printf("\n");
}
    
/* 
   We're going to take some pointers from Martin Tracy and zenFORTH.
   `last-link' is a 2variable that stores (link, chain) of the last
   named word defined.  We don't actually link it into the dictionary
   until we think it's safe to do so, thereby obviating the need for
   `smudge'.

   2variable last-link  ( &link &chain)
   variable last-code   ( pointer to last-compiled code field)
   variable last-word   ( last compiled word)

   : show     last-link 2@  ( &link chain)  !   ; 
   : use      ( code -)  last-code @ !  ;
   : patch    pop @  use   ; 

   : ?unique  ( a u - a u)
   2dup current @  ?unique-hook  find  if
   drop  2dup  fd-out @ push  >stderr  type  ."  again.  "  pop writes 
   then   2drop ;

   : code,   0 , ( lex)  here last-code !  0 , ( code)  ;
   : token,  ( - 'link)  token  ?unique  here  scrabble>  allot  ;
   : link,   ( here)  current @  dup @ ,  last-link 2!  ;
   : head,   token,  link,  ;

   : name        head,  code,  ;
   : noname   0 align,  code,  ;

   ( Dictionary structure)
   : link>name   1- ( char-)  dup c@  swap over -  swap  ;
   : >link  ( body - link)  cell- cell- cell- ;
   : link>  ( link - body)  cell+ cell+ cell+ ;
   : >name  ( body - a u)   >link  link>name  ;
   : >lex   ( body - lex)   cell- cell-  ;
   : >code  ( body - code)  cell-  ;
*/

#ifdef DEBUG_FOO
/*
 * daf: stripped the mu_ off the names of these routines, since they conform
 * to C stack API rather than muforth stack API.
 */

/*
 * find_pde_code_range()
 *
 * This routine will locate the supplied address to a specific
 * word within a given chain.  The caller (mu_fund_pde_by_addr()) will
 * determine if a word is in the compiler chain or the forth chain.
 * It does this by determining which has the lowest gap between
 * addr and the start of the function of each chain in which
 * the addr matches.
 */
static struct dict_entry *find_pde_code_range(struct dict_entry *chain, cell addr)
{
    struct dict_entry *pde, *best;
    u_int32_t closest, delta;

    /*
     * First pass: Check for direct hit
     */
    pde = chain;
    best = NULL;
    closest = 0x7FFF;
    do {
        if (addr == (cell) pde->code)
            return pde;
        delta = addr - (cell) pde->code;
        if (delta < closest) {
            best = pde;
            closest = delta;
        }
        pde = pde->link;
    } while (pde);

    if (closest != 0x7FFF)
        return best;

    return NULL;
}

/*
 * find_pde_by_addr()
 *
 * This routine calls mu_find_pde_code_range() to identify a possible
 * pde in both chains.  If nothing is found, it returns NULL.  Else,
 * it returns the pde for which the code segment is most likely the
 * the correct code segment for this address.  The heuristic for this
 * is, if the addr does not match the start address of either pde,
 * which pde has the smallest difference between the supplied addr
 * and the start of the code for that pde.
 */
static struct dict_entry *find_pde_by_addr(cell addr)
{
    struct dict_entry *pde_compiler, *pde_forth, *pde;
    u_int32_t comp_code, forth_code;

    pde_compiler = find_pde_code_range(compiler_chain, addr);
    pde_forth = find_pde_code_range(forth_chain, addr);

    if (!pde_compiler && !pde_forth)
        return NULL;

    if (pde_compiler)
        comp_code = (cell) pde_compiler->code;
    else
        comp_code = 0;

    if (pde_forth)
        forth_code = (cell) pde_forth->code;
    else
        forth_code = 0;

    if (comp_code  == addr)
        pde = pde_compiler;
    else if (forth_code == addr)
        pde = pde_forth;
    else {
        /*
         * This is the heuristic: we check to see which code
         * segment start is the closet to the supplied addr.
         */
        if ((addr - comp_code) < (addr - forth_code))
            pde = pde_compiler;
        else
            pde = pde_forth;
    }

    return pde;
}

/*
 * snprint_func_name()
 *
 * Given an address, this routine will print the name of the muforth word
 * which is associated with that address.  Also, it will print an offset
 * value if the address isn't the start of the code segment.
 *
 * It returns the base address of the muforth word.
 */
cell snprint_func_name(char *line, int size, cell addr)
{
    struct dict_entry *pde;
    cell code;
    int idx;

    pde = find_pde_by_addr(addr);

    if (!pde) {
        snprintf(line, size, "Unknown function: address %p", (cell *) addr);
        return addr;
    }

    code = (cell) pde->code;

    idx = snprintf(line, size, "%.*s", pde->length, pde->name);
    if (code != addr) {
        snprintf(line +idx, size -idx, " + %d", addr - code);
    }

    return code;
}
#endif /* DEBUG */
