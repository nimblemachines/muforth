/*
 * debugger.c
 *
 * This file implements a muforth debugger.
 *
 * The debugger has a basic interface and users can over-ride it with
 * their own implementations.
 */
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
 * And, my theory is that I'll implement this simply enough that
 * we can port it to muForth soon.
 *
 * To send commands to the target, call through the dbg_send vector.
 * To pick up responses from the target, call through the dbg_recv vector.
 * These vectors need to be implemented by by routines that can send
 * and receive bytes to/from the target machine.
 *
 * On the target machine there needs to be a nub.  The nub responds to
 * commands and sends responds.  The nub does not initiate new commands
 * to the host.  The only exception to that rule is that the nub
 * will send data to the host indicating that the target is in the debugger.
 *
 * If this all feels like gdb and its nub that's because I've modeled this after
 * that structure.
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
 * Already I'm sensing trouble down this path.  The problem is that the
 * text of the commands is long.  This means that the nub will have lots
 * of bytes devoted to the parsing the commands.  Shorter commands is better.
 * 
 * Given that, here's another idea: short one byte commands.
 *
 * Here are my proposed commands:
 *   One byte commands:
 *     i   -- Fetch the target machine's information.
 *            The response to this is a pretty long string of terms
 *            each separated by semicolon's:
 *         b or l   -- big or little endian
 *         cpu-arch -- a short text string identifying the cpu arch:
 *                     i386, arm7, mippy, 6809, 68000, etc.
 *         dram<base>,<length>  -- This is a declarative of the range
 *                                 of dram on the machine.  There may
 *                                 be multiple such declaratives.  Also,
 *                                 these may not be exaustive.
 *         heap<base>,<length>  -- The range of the heap.  Searches for
 *                                 stuff in the heap won't extend beyond
 *                                 these boundaries.
 *         stack<base>,<length> -- The stack range.  In this case <base> is
 *                                 the address when there are no elements
 *                                 on the stack.  <length> indicates the
 *                                 number of bytes available for the stack
 *                                 to grow.  Also, if <length> is an
 *                                 empty string, then the stack is presumed
 *                                 to be able to grow indefinitely, which
 *                                 will likely be the case.
 *         rstack<base>,<length> -- The return stack range.  All said about
 *                                 the data stack, above, is true here.  In
 *                                 addition the rstack and the stack may
 *                                 have overlapping ranges.  Some systems are
 *                                 designed this way.
 *         pc<addr>              -- The current program counter.
 *         sp<addr>              -- The current stack pointer
 *         rp<addr>              -- The current return pointer
 *         <cpu-reg><value>      -- Some number (possibly 0) of cpu specific
 *                                  registers and their current values.
 *
 *     c   -- continue execution form here
 * 
 * Other commands:
 *
 * m<addr>,<length>;   -- read memory at <addr> for <length> bytes.
 * M<addr>:<datum>[,<datum>]*; -- set memory at <addr> with datum.
 *                                additiona data is placed at successive
 *                                addresses.
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
#endif
