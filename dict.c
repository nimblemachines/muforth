/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* dictionary */

#include "muforth.h"

struct dict_entry
{
    struct dict_entry *link;
    void *code;
    unsigned char name_len;
    char name[0];
};

/* no reason to put these in an array unless we have marker & empty */
struct dict_entry *dict_chain[8];	/* dictionary "chains" of words */

struct dict_entry **current_chain = &dict_chain[0];
					/* current chain to compile into */
void (*name_hook)() = nope;		/* called when a name is created */

/* bogus C-style dictionary init */
struct inm			/* "initial name" */
{
    char *name;
    void (*code)();
};

struct inm initial_forth[] = {
    { "version", push_version },
    { "build-time", push_build_time },
    { ":", colon },
    { "name", make_new_name },
    { "'name-hook", push_tick_name_hook },
    { ".forth.", forth_chain },
    { ".compiler.", compiler_chain },
    { "current", push_current },
    { "'number", push_tick_number },
    { "'number,", push_tick_number_comma },
    { "last", push_last_call },
    { "h", push_h },
    { "r", push_r },
    { "s0", push_s0 },
    { "'sp", push_sp },
    { "catch", catch },
    { "throw", throw },
    { "argc", push_argc },
    { "argv", push_argv },
    { "token", token },
    { "parse", parse },
    { "-\"find", minus_quote_find },
    { "interpret", interpret },
    { "evaluate", evaluate },
    { "open-file", open_file },
    { "r/o", push_ro_flags },
    { "r/w", push_rw_flags },
    { "w/o", push_create_flags },
    { "close-file", close_file },
    { "mmap-file", mmap_file },    
    { "load-file", load_file },
    { "literal", compile_literal },
    { "literal-load", compile_literal_load },
    { "literal-push", compile_literal_push },
    { "compile,", compile_call },
    { "state", push_state },
    { "-]", minus_rbracket },
    { "parsed", push_parsed },
    { "huh?", huh },
    { "complain", complain },
    { "load-sp", compile_sp_to_eax },
    { "0branch", compile_zbranch },
    { "branch", compile_branch },
    { "_?for", compile_qfor },
    { "_next", compile_next },
    { "scrabble", scrabble },
    { "nope", nope },
    { "zzz", zzz },
    { "+", add },
    { "and", and },
    { "or", or },
    { "xor", xor },
    { "negate", negate },
    { "invert", invert },
    { "u<", uless },
    { "0<", zless },
    { "0=", zequal },
    { "<", less },
    { "2*", two_star },
    { "2/", two_slash },
    { "u2/", two_slash_unsigned },
    { "<<", shift_left },
    { ">>", shift_right },
    { "u>>", shift_right_unsigned },
    { "d+", dplus },
    { "dnegate", dnegate },
    { "um*", um_star },
    { "m*", m_star },
    { "um/mod", um_slash_mod },
    { "fm/mod", fm_slash_mod },
    { "jump", jump },
    { "@", fetch },
    { "c@", cfetch },
    { "!", store },
    { "c!", cstore },
    { "+!", plus_store },
    { "rot", rot },
    { "-rot", minus_rot },
    { "dup", dupe },
    { "nip", nip },
    { "swap", swap },
    { "over", over },
    { "tuck", tuck },
    { "1", const_one },
    { "\"=", string_equal },
    { "read", read_carefully },
    { "write", write_carefully },
    { "sp@", sp_fetch },
    { "sp!", sp_store },
    { "cmove", cmove },
    { "string-length", string_length },
    { "local-time", local_time },
    { "utc", global_time },
    { "clock", push_clock },
#ifdef __FreeBSD__
    { "pci", pci_open },
    { "pci@", pci_read },
#endif
    /* tty.c */
    { "get-termios", get_termios },
    { "set-termios", set_termios },
    { "set-raw", set_termios_raw },
    { "set-min-time", set_termios_min_time },
    { "set-speed", set_termios_speed },

    /* select.c */
    { "fd-zero", my_fd_zero },
    { "fd-set", my_fd_set },
    { "fd-clr", my_fd_clr },
    { "fd-in-set?", my_fd_isset },
    { "select", my_select },

    { "bye", bye },
    { NULL, NULL }
};

struct inm initial_compiler[] = {
    { ";", semicolon },
    { "-;", minus_semicolon },
    { "^", compile_return },
    { "[", lbracket },
    { "drop", compile_drop },
    { "2drop", compile_2drop },
    { "push", compile_push_to_r },
    { "2push", compile_2push_to_r },
    { "pop", compile_pop_from_r },
    { "2pop", compile_2pop_from_r },
    { "r@", compile_copy_from_r },
    { "shunt", compile_shunt },
    { NULL, NULL }
};

void push_current()
{
    PUSH(&current_chain);
}

void forth_chain()
{
    PUSH(&dict_chain[0]);
}

void compiler_chain()
{
    PUSH(&dict_chain[1]);
}

/* -"find  ( a u chain - a u t | code f) */
void minus_quote_find()
{
    char *token = (char *) STK(2);
    int len = STK(1);
    struct dict_entry *pde = (struct dict_entry *) TOP;

    while ((pde = pde->link) != NULL)
    {
	if (pde->name_len != len) continue;
	if (memcmp(&pde->name, token, len) != 0) continue;
	STK(2) = (int) pde->code;
	STK(1) = 0;
	DROP(1);
	return;
    }
    TOP = -1;
}

struct dict_entry *compile_dict_entry(
    struct dict_entry **ppde, struct string *ptok, void *pcode)
{
    struct dict_entry *pde = (struct dict_entry *) pnm;

    /* compile link */
    pde->link = *ppde;

    /* link onto front of chain */
    *ppde = pde;

    /* compile code pointer */
    pde->code = pcode;

    /* copy name string */
    pde->name_len = ptok->len;
    memcpy(&pde->name, ptok->data, ptok->len);

    /* align */
    pnm = &pde->name[0] + ptok->len + ALIGN_SIZE - 1;
    (int) pnm &= -ALIGN_SIZE;

    return pde;
}

/* called from Forth */
void compile_name()
{
    struct string token;
    struct dict_entry *pde;

    token.data = (char *) STK(1);
    token.len = TOP;
    DROP(2);

    pde = compile_dict_entry(current_chain, &token, pcd);
}

/* called _only_ by initialization code, _not_ from Forth */
void compile_init_name(struct dict_entry **pchain, struct inm *pinm)
{
    struct string token;
    struct dict_entry *pde;

    token.data = pinm->name;
    token.len = strlen(pinm->name);
    pde = compile_dict_entry(pchain, &token, pinm->code);
}


void make_new_name()
{
    token();
    (*name_hook)();
    compile_name();
}

void push_tick_name_hook()
{
    PUSH(&name_hook);
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
: patch	   pop @  use	; 

: ?unique  ( a u - a u)
   2dup current @  ?unique-hook  -"find  if  2drop exit  then
   drop  2dup  fd-out @ push  >stderr  type  ."  again.  "  pop writes ;

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

static void init_chain(struct dict_entry **pchain, struct inm *pinm)
{
    while (pinm->name != NULL)
	compile_init_name(pchain, pinm++);
	
}

void init_dict()
{
    init_chain(&dict_chain[0], initial_forth);
    init_chain(&dict_chain[1], initial_compiler);
}



