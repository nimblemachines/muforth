/*
 * debugger.c
 *
 * This file implements a muforth debugger.
 *
 * The debugger has a basic interface and users can over-ride it with
 * their own implementations.
 */
#ifdef DEBUG
#include "muforth.h"
#include "opcode.h"
#include <stdio.h>
#include <stdlib.h>

static void simple_debugger(void);
extern cell_t snprint_func_name(char *str, int size, cell_t addr);

/**********************************************
 *
 * debugger structure
 *
 * The debugger is predicated on the theory that it knows very little
 * about the target machine.  Therefore, it querries the target machine
 * for the following information:
 * 1) heap base, heap length -- These are the dictionary heaps
 * 2) stack base, stack length -- The region of the data stack
 * 3) return stack base, length -- Return stack region
 * 4) read/set muForth registers -- PC, SP, RP
 * 5) read processor architecture -- The debugger doesn't even know that!
 * 6) read/set processor specific registers -- Once the cpu is known then processor
 *    specific registers can be read/set
 * 7) read/set memory
 *
 * The host (and client) will build up a complete packet and then send
 * it to the target (over what-ever interconnect: RS232, USB, etc.).
 * When the host sends the packet it will block waiting for the response.
 * When the target receives the packet it will call through some 'eval'
 * engine (with the current chain set to the debugger).  Upon evaluating
 * packet, it will generate a response packet.  That packet will be sent
 * back to the host.  The host, too, will 'eval' the packet.  The results
 * from that operation will be (for example) data on the stack that can
 * printed along with some formatting and context.
 *
 * On the target machine there needs to be a nub.  The nub responds to
 * commands and sends responds.  The nub does not initiate new commands
 * to the host.  The only exception to that rule is that the nub
 * will send data to the host indicating that the target is in the debugger.
 *
 * Oh, here's the big fun.  There are two data stacks and two return stacks.
 * This is because when we're debugging local code, we can't use the same stack
 * that is currently in use.  Therefore we have to switch stacks.  However,
 * when we're debugging a target machine then we don't need to use a separate
 * stack.  What we want here are two ways to enter the debugger.  In one
 * case we'll have entered the debugger because code on the local machine
 * excepted or tripped some how.  The other entry is when we want to connect
 * to the target.  In that case there is no need to switch stacks (in fact,
 * switching stacks there would be wrong).
 *
 * The trigger will be if we are entered via a call to mu_dbg() or via the
 * word mu_dbg_connect().  Other than that, they're identical implementations.
 * 
 * One of my ideas is to send muForth-like commands over the wire.  In this mode
 * an entire command would be sent as a packet.  The reason this is important is
 * the sender cannot send one packet and then later send another packet because
 * the target will be waiting for the first command.  But, after that command
 * arrives, it isn't going to wait for additional commands.  It's going to
 * execute those commands and then generate a response.  That response will start
 * going out over the wire.  The host had better be listening for the response!
 * Another way to look at this dilemma is that the host may have the benefit of
 * an O/S with processes to ship data over to the target, but the target does not.
 * The target is single threaded and -either- listening for the request or
 * responding to one.  Period.
 *
 * Given that idea, here are my target->nub request commands:
 *
 * endian?
 * cpu-arch?
 * heap-base?
 * heap-end?
 * stack-base?
 * stack-end?
 *
 */

/*
 * The debugger is vectored, so you can implement
 * your own debugger if you wish.
 * Use the muForth word "debugger" to place the address of this
 * pointer onto the stack.
 */
cell_t debugger;
cell_t dbg_send;
cell_t dbg_recv;

/*********************************************************
 *
 * The basic strategy of this debugger is to declare separate
 * host and target semantics.  The host is defined as the
 * machine on which the debugger is running.  The target is
 * defined as the machine on which the debugger nub is
 * running.  When the host and the target are the same,
 * the line seems blurry.  The best way to think about the
 * design of this debugger is to imagine that you've got
 * a muForth system running on FreeBSD on an Intel processor
 * but you're trying to debug some ARM code that you've
 * built and is running.
 *
 * The host system knows that there are well defined
 * muForth 'registers' that need to be obtained from the
 * target system:
 *    PC  - Program counter
 *    SP  - Data stack pointer
 *    S0  - The base address of the data stack (used for
 *          calculating the stack depth)
 *    RP  - Return stack pointer
 *    R0  - The base address of the return stack
 *
 * In addition, the host system understands that there
 * may very well be processor specific registers that
 * the user may want to look at and possibly change.
 */

/*
 * dbg()
 * 
 * Ultimately, this routine will call through a pointer to a,
 * possibly, user defined debugger.  Right now, we use the
 * supplied debugger.
 *
 * The debugger is called with the address of the instruction
 * at which the debugger was invoked on top of the stack.
 */
void mu_dbg(void)
{
	if (debugger) {
		execute(debugger);
	} else {
		simple_debugger();
	}
}

void mu_debugger(void)
{
	PUSH((cell_t) &debugger);
}

static void simple_debugger(void)
{
	cell_t *pc;

        pc = (cell_t *) RPOP;
	PUSH(pc);
	mu_disassemble();
	mu_print_stacks();
	mu_read_carefully();

	RPUSH((cell_t) pc);
}

/*************************************************
 * mu_print_stacks()
 *
 * These functions print out the user and return stacks.
 *
 */
static void print_stack(cell_t *sp, int count)
{
	int i;

	if (count == 0) {
		printf("empty");
		fflush(stdout);
		return;
	}

	if (count > 4)
		count = 4;

	for (i = 0; i < count; i++)
		printf("%x  ", sp[i]);
	fflush(stdout);
}

void mu_print_stacks(void)
{
	printf("(  #%d: ", S0-sp);
	print_stack(sp, S0-sp);
	printf(")  ");
	if ((R0-rsp > 1) || (RTOP != 0)) {
		printf("[  #%d: ", R0-rsp);
		print_stack(rsp, R0-rsp);
		printf("]");
	}
	printf("\n");
	fflush(stdout);
}

/***********************************************
 * get_opcode_name()
 * get_opcode_cellcount()
 *
 * These functions return information about opcodes
 */
static char *get_opcode_name(cell_t opcode)
{
	char *opnames[] = {
		"brk", "nop",
		"call", "jump", "exec", "ret",
		"literal", "lit_load", "lit_push", 
		"branch", "zbranch", "=zbranch", 
		"?for", "next", "i", "j", 
		"/mod", "u/mod", "mul", "umul", 
		"drop", "2drop", ">r", "2>r", "r>", "2r>", "@r", "shunt"
	};
	char *opstr;

	if (opcode <= SHUNT) {
		opstr = opnames[opcode];
	} else {
		opstr = NULL;
	}

	return opstr;
}

static int get_opcode_cellcount(cell_t opcode)
{
	int opcode_cellcount[] = {
		1, 1, 
		2, 2, 1, 1, 
		2, 2, 1, 
		2, 2, 2, 
		2, 2, 1, 1, 
		1, 1, 1, 1, 
		1, 1, 1, 1, 1, 1, 1, 1
	};

	if (opcode <= SHUNT)
		return opcode_cellcount[opcode];
	else 
		return 0;
}

/***************************************************************
 * disassemble5()
 * mu_disassemble()
 *
 * These routines disassemble segments of code
 */
#if 0
static void disassemble5(cell_t *pc)
{
	int i;

	for (i = 0; i < 5; i++) {
		PUSH((cell_t) pc);
		mu_disassemble();
		pc = (cell_t *) POP;
		printf("\n");
	}
	printf("%p:\n", pc);
}
#endif /* 0 */

#define LINESTR		line+idx, sizeof(line)-idx
#define CODE_COL	10
#define OPERAND_COL	20
#define COMMENT_COL	40

void mu_disassemble(void)
{
	cell_t opcode, *pc;
	char *opstr;
	char line[80];
	int idx;

	pc = (cell_t *) POP;

	idx = 0;  /* Used in the LINESTR macro, below */
	idx = snprintf(LINESTR, "%p: ", pc);
	if (idx < CODE_COL)
		idx += snprintf("%*s", CODE_COL - idx, " ");

	opcode = *pc;
	opstr = get_opcode_name(opcode);
	if (!opstr) {
		idx += snprintf(LINESTR, "unknown opcode: %x", opcode);
		idx += snprintf(LINESTR, "%*s", (int) (COMMENT_COL-strlen(line)), " ");
		printf("%s", line);
		fflush(stdout);
		return;
	}

	idx += snprintf(LINESTR, "%s", opstr);
	idx += snprintf(LINESTR, "%*s", (int) (OPERAND_COL - strlen(line)), " ");

	switch(opcode) {
	case CALL:
	case JUMP:
		snprint_func_name(LINESTR, pc[1]);
		idx = strlen(line);
		idx += snprintf(LINESTR, "   @%p", (cell_t *) pc[1]);
		break;

	case BRANCH:
	case ZBRANCH:
	case _ZBRANCH:
	case QFOR:
	case NEXT:
		idx += snprintf(LINESTR, "   %p", (cell_t *) pc[1]);
		break;

	case LITERAL:
	case LIT_LOAD:
		idx += snprintf(LINESTR, "#%d", pc[1]);
		break;

	default:
		break;
	}

	idx += snprintf(LINESTR, "%*s", (int) (COMMENT_COL-strlen(line)), " ");
	printf("%s", line);
	fflush(stdout);
	pc += get_opcode_cellcount(*pc);
	PUSH((cell_t) pc);
}

#if 0
static void print_opcode_histogram(void);
static int opcode_histogram[128];
static void print_opcode_histogram(void)
{
	int i;
	char *opstr;

	for (i = 0; i < 128; i++) {
		if (opcode_histogram[i] == 0)
			continue;
		opstr = get_opcode_name(i);
		printf("%s:\t%d", opstr, opcode_histogram[i]);
	}
}
#endif /* 0 */
#endif /* DEBUG */
