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

#define ALIGN_SIZE  sizeof(int)

/* string is a "normal" string: pointer to the first character, and length. */
struct string
{
    char *data;
    size_t len;
};

/*
 * text is an odd beast. It's intended for parsing, and other applications
 * that scan a piece of text. To make this more efficient we store a pointer
 * to the _end_ of the text, and a _negative_ offset to its start, rather than
 * the way struct string works.
 */
struct text
{
    char *end;
    ssize_t start;	/* ssize_t is a _signed_ type */
};

extern struct string parsed;	/* for errors */

extern int stack[];
extern int *sp;

extern int  cmd_line_argc;
extern char **cmd_line_argv;

extern u_int8_t *pnm0, *pcd0, *pdt0;	/* ptrs to name, code, & data spaces */
extern u_int8_t *pnm, *pcd, *pdt;    /* ptrs to next free byte in each space */

void (*number)();
void (*number_comma)();
void (*name_hook)();			/* called when a name is created */

/* declare common functions */
/* error.c */
void die(const char *msg);
void catch(void);
void throw(void);

/* file.c */
void create_file(void);
void open_file(void);
void push_ro_flags(void);
void push_rw_flags(void);
void close_file(void);
void mmap_file(void);
void load_file(void);
void read_carefully(void);  /* XXX: temporary */
void write_carefully(void); /* XXX: temporary */


/* i386.c */
void compile_call(void);
void compile_jump(void);
void compile_return(void);
void push_last_call(void);
void compile_drop(void);
void compile_2drop(void);
void compile_literal(void);
void compile_literal_load(void);
void compile_literal_push(void);
void compile_sp_to_eax(void);
void compile_zbranch(void);
void compile_branch(void);
void compile_push_to_r(void);
void compile_2push_to_r(void);
void compile_shunt(void);
void compile_pop_from_r(void);
void compile_2pop_from_r(void);
void compile_copy_from_r(void);
void compile_qfor(void);
void compile_next(void);
void dplus(void);
void dnegate(void);
void um_star(void);
void m_star(void);
void um_slash_mod(void);
void fm_slash_mod(void);
void jump(void);

/* interpret.c */
void start_up(void);
void nope(void);
void zzz(void);
void token(void);
void parse(void);
void huh(void);
void complain(void);
void depth(void);
void interpret(void);
void evaluate(void);
void push_tick_number(void);
void push_tick_number_comma(void);
void push_state(void);
void lbracket(void);
void minus_rbracket(void);
void push_parsed(void);
void bye(void);

/* compile.c */
void push_h(void);
void push_r(void);
void push_s0(void);
void push_sp(void);
void scrabble(void);
void colon(void);
void semicolon(void);
void minus_semicolon(void);

/* dict.c */
void definitions(void);
void forth_chain(void);
void compiler_chain(void);
void push_current(void);
void minus_quote_find(void);
void show(void);
void make_new_name(void);
void push_tick_name_hook(void);
void init_dict(void);

/* buf.c */

/* kernel.c */
void add(void);
void and(void);
void or(void);
void xor(void);
void negate(void);
void invert(void);
void two_star(void);
void two_slash(void);
void two_slash_unsigned(void);
void shift_left(void);
void shift_right(void);
void shift_right_unsigned(void);
void fetch(void);
void cfetch(void);
void store(void);
void cstore(void);
void plus_store(void);
void drop(void);
void two_drop(void);
void const_one(void);
void rot(void);
void minus_rot(void);
void dupe(void);
void nip(void);
void swap(void);
void over(void);
void tuck(void);
void string_equal(void);
void uless(void);
void zless(void);
void zequal(void);
void less(void);
void sp_fetch(void);
void sp_store(void);
void cmove(void);
void string_length(void);
void push_version(void);
void push_build_time(void);
void push_argc(void);
void push_argv(void);

/* time.c */
void local_time(void);
void global_time(void);
void push_clock(void);

/* pci.c */
#ifdef __FreeBSD__
void pci_read(void);
void pci_open(void);
#endif

/* tty.c */
void get_termios(void);
void set_termios(void);
void set_termios_raw(void);
void set_termios_min_time(void);
void set_termios_speed(void);

/* select.c */
void my_fd_zero(void);
void my_fd_set(void);
void my_fd_clr(void);
void my_fd_isset(void);
void my_select(void);
