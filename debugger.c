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

cell_t debugger;

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
