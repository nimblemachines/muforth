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

#include <sys/types.h>
#include <string.h>
    
/* data stack */
#define STACK_SIZE 4096
#define STACK_SAFETY 256
#define S0 &stack[STACK_SIZE - STACK_SAFETY]

/* gcc generates stupid code using this def'n */
/* #define PUSH(n) 	(*--sp = (int)(n)) */
#define PUSH(n)		(sp[-1] = (int)(n), --sp)
#define POP		(*sp++)
#define STK(n)  	(sp[n])
#define TOP		STK(0)
#define DROP(n)		(sp += n)
#define EXECUTE		(*(void (*)()) POP)()

#ifdef DEBUG
#include <stdio.h>
#define BUG printf
#else
#define BUG
#endif

#define ALIGN_SIZE	sizeof(int)
#define ALIGNED(x)	(((int)(x) + ALIGN_SIZE - 1) & -ALIGN_SIZE)

/*
 * struct string is a "normal" string: pointer to the first character,
 * and length.
 */
struct string
{
    char *data;
    size_t length;
};

/*
 * struct text is an odd beast. It's intended for parsing, and other
 * applications that scan a piece of text. To make this more efficient
 * we store a pointer to the _end_ of the text, and a _negative_
 * offset to its start, rather than the way struct string works.
 */

struct text
{
    char *end;
    ssize_t start;	/* ssize_t is a _signed_ type */
};

struct counted_string
{
    size_t length;	/* cell-sized length, unlike older Forths */
    char data[0];
};

#define COUNTED_STRING(x)	{ strlen(x), x }

extern struct string parsed;	/* for errors */

extern int stack[];
extern int *sp;

extern int  cmd_line_argc;
extern char **cmd_line_argv;

extern u_int8_t *pnm0, *pcd0, *pdt0;	/* ptrs to name, code, & data spaces */
extern u_int8_t *pnm, *pcd, *pdt;    /* ptrs to next free byte in each space */

extern void (*mu_number)();
extern void (*mu_number_comma)();
extern void (*mu_name_hook)();		/* called when a name is created */

/* XXX: Gross hack alert! */
extern char *ate_the_stack;
extern char *isnt_defined;
extern char *version;

/* declare common functions */
/* muforth.c */
void mu_push_name_size(void);
void mu_push_code_size(void);
void mu_push_data_size(void);
void mu_push_command_line(void);
void mu_push_version(void);
void mu_push_build_time(void);

/* error.c */
void die(const char *msg);
void mu_catch(void);
void mu_throw(void);
char *counted_strerror(void);

/* file.c */
void mu_create_file(void);
void mu_open_file(void);
void mu_push_ro_flags(void);
void mu_push_rw_flags(void);
void mu_close_file(void);
void mu_mmap_file(void);
void mu_load_file(void);
void mu_readable_q();
void mu_read_carefully(void);  /* XXX: temporary */
void mu_write_carefully(void); /* XXX: temporary */


/* i386.c */
void mu_compile_call(void);
void mu_resolve(void);
void mu_compile_jump(void);
void mu_compile_return(void);
void mu_push_last_call(void);
void mu_compile_drop(void);
void mu_compile_2drop(void);
void mu_compile_literal_load(void);
void mu_compile_literal_push(void);
void mu_compile_destructive_zbranch();
void mu_compile_nondestructive_zbranch();
void mu_compile_branch(void);
void mu_compile_push_to_r(void);
void mu_compile_2push_to_r(void);
void mu_compile_shunt(void);
void mu_compile_pop_from_r(void);
void mu_compile_2pop_from_r(void);
void mu_compile_copy_from_r(void);
void mu_compile_qfor(void);
void mu_compile_next(void);
void mu_dplus(void);
void mu_dnegate(void);
void mu_um_star(void);
void mu_m_star(void);
void mu_um_slash_mod(void);
void mu_fm_slash_mod(void);
void mu_jump(void);

/* interpret.c */
void mu_start_up(void);
void mu_nope(void);
void mu_zzz(void);
void mu_token(void);
void mu_parse(void);
void mu_huh(void);
void mu_complain(void);
void mu_depth(void);
void mu_interpret(void);
void mu_evaluate(void);
void mu_push_tick_number(void);
void mu_push_tick_number_comma(void);
void mu_push_state(void);
void mu_lbracket(void);
void mu_minus_rbracket(void);
void mu_push_parsed(void);
void mu_bye(void);

/* compile.c */
void mu_push_h(void);
void mu_push_r(void);
void mu_push_s0(void);
void mu_push_sp(void);
void mu_scrabble(void);
void mu_colon(void);
void mu_semicolon(void);
void mu_minus_semicolon(void);
char *to_counted_string(char *);

/* dict.c */
void mu_definitions(void);
void mu_push_forth_chain(void);
void mu_push_compiler_chain(void);
void mu_push_current(void);
void mu_find(void);
void mu_show(void);
void mu_make_new_name(void);
void mu_push_tick_name_hook(void);
void init_dict(void);

/* buf.c */

/* kernel.c */
void mu_add(void);
void mu_and(void);
void mu_or(void);
void mu_xor(void);
void mu_negate(void);
void mu_invert(void);
void mu_two_star(void);
void mu_two_slash(void);
void mu_two_slash_unsigned(void);
void mu_shift_left(void);
void mu_shift_right(void);
void mu_shift_right_unsigned(void);
void mu_fetch(void);
void mu_cfetch(void);
void mu_store(void);
void mu_cstore(void);
void mu_plus_store(void);
void mu_drop(void);
void mu_two_drop(void);
void mu_rot(void);
void mu_minus_rot(void);
void mu_dupe(void);
void mu_nip(void);
void mu_swap(void);
void mu_over(void);
void mu_tuck(void);
void mu_string_equal(void);
void mu_uless(void);
void mu_zless(void);
void mu_zequal(void);
void mu_less(void);
void mu_sp_fetch(void);
void mu_sp_store(void);
void mu_cmove(void);

/* time.c */
void mu_local_time(void);
void mu_global_time(void);
void mu_push_clock(void);

/* pci.c */
#ifdef __FreeBSD__
void mu_pci_read(void);
void mu_pci_open(void);
#endif

/* tty.c */
void mu_get_termios(void);
void mu_set_termios(void);
void mu_set_termios_raw(void);
void mu_set_termios_min_time(void);
void mu_set_termios_speed(void);

/* select.c */
void mu_fd_zero(void);
void mu_fd_set(void);
void mu_fd_clr(void);
void mu_fd_isset(void);
void mu_select(void);
