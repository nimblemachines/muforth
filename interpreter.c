/*
 * op_interpreter.c
 *
 * This file is a simple interpreter for the code
 * compiled by op_compiler.c.
 *
 * The R stack is a simple synthetic stack here.  It is used
 * to hold the call chain for called words and the index for
 * for/next loops.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "muforth.h"
#include "opcode.h"

typedef void (*forth_func_t)(void);
static void interpreter(cell_t pc);

#ifdef DEBUG
extern cell_t print_func_name(cell_t addr);
extern cell_t snprint_func_name(char *str, int size, cell_t addr);
static void disassemble(cell_t *pc);
static void disasm_blanks(void);

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

static void print_stacks(void)
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
#endif /* DEBUG */

static int is_func_pcd(cell_t target)
{
	code_t *pc;
	pc = (code_t *) target;
	return (pc >= &pcd0[0]) && (pc < &pcd0[PCD_SIZE / sizeof(*pcd0)]);
}

static void call(cell_t target)
{
	forth_func_t func = (forth_func_t) target;

#ifdef DEBUG
	print_stacks();
#endif /* DEBUG */

	func();

#ifdef DEBUG
	disasm_blanks();
#endif /* DEBUG */
}

void execute(cell_t target)
{
	if (is_func_pcd(target)) {
		interpreter(target);
	} else {
		call(target);
	}
}

#ifdef DEBUG
static 	int show_calls = 0;
static void show_call(cell_t pc)
{
	if (show_calls) {
		printf("CALL: ");
		print_func_name(pc);
		printf(" (%p)\n", (cell_t *) pc);
	}
}
#else /* !DEBUG */
#define show_call(x)		/* Nothing in regular muforth images */
#endif /* DEBUG */

static cell_t *op_jump(cell_t pc)
{
	show_call(pc);
	if (!is_func_pcd(pc)) {
		call(pc);
		pc = RPOP;
	}
	return (cell_t *) pc;
}

static cell_t *op_zbranch(cell_t *pc, cell_t tos)
{
	if (tos == 0) {
		pc = (cell_t *) *pc;
	} else {
		/* skip the branch target */
		pc ++;
	}
	return pc;
}

static cell_t *op_qfor(cell_t *pc)
{
	cell_t count;

	count = POP;
	if (!count)
		pc = (cell_t *) *pc;
	else {
		/* skip the branch target */
		pc ++;
		RPUSH(count);
	}
	return pc;
}

static cell_t *op_next(cell_t *pc)
{
	*rsp -= 1;
	if (RTOP) {
		pc = (cell_t *) *pc;
	} else {
		/* skip the branch target */
		pc ++;
		(void) RPOP;
	}
	return pc;
}


static int hold_literal = 0;
static void interpreter(cell_t codep)
{
	cell_t  *pc;

	pc = (cell_t *) codep;

	/*
	 * We exit the interpreter when we return to 0
	 */
	RPUSH(0);

	do {
#ifdef DEBUG
		disassemble(pc);
#endif /* DEBUG */

		switch(*pc++) {
		case NOP:					break;

		case JUMP: pc = op_jump(*pc);			break;
		case EXEC: pc = op_jump(POP); 			break;
		case CALL: RPUSH(pc +1); pc = op_jump(*pc);	break;
		case RET:  pc = (cell_t *) RPOP;		break;

		case LITERAL:  PUSH((cell_t) *pc++);		break;
		case LIT_LOAD: hold_literal = *pc++;		break;
		case LIT_PUSH: PUSH(hold_literal);		break;

		case BRANCH:   pc = (cell_t *) *pc;		break;
		case ZBRANCH:  pc = op_zbranch(pc, POP);	break;
		case _ZBRANCH: pc = op_zbranch(pc, TOP);	break;

		case QFOR: pc = op_qfor(pc);			break;
		case NEXT: pc = op_next(pc);			break;

		case I: PUSH(RTOP);				break;
		case J: PUSH(rsp[1]);				break;

		case DROP:  DROP(1);				break;
		case DROP2: DROP(2);				break;

		case PUSH_TO_R:	  RPUSH(POP);			break;
		case POP_FROM_R:  PUSH(RPOP);			break;
		case PUSH2_TO_R:  RPUSH(POP); RPUSH(POP);	break;
		case POP2_FROM_R: PUSH(RPOP); PUSH(RPOP);	break;
		case COPY_FROM_R: PUSH(RTOP);			break;
		case SHUNT:       RDROP(1);			break;

		default:
			pc --;
			printf("?ILLEGAL INSTRUCTION (opcode %d) at address %p\n", *pc, pc);
			mu_zzz();
			exit(-1);
			break;
		}
#ifdef DEBUG
		print_stacks();
#endif /* DEBUG */
	} while (pc);
}

#ifdef DEBUG
#define INV1		0,
#define INV2		INV1 INV1
#define INV4		INV2 INV2
#define INV7		INV4 INV2 INV1
#define INV8		INV4 INV4
#define INV12		INV8 INV4
#define INV13		INV8 INV4 INV1
#define INV14		INV7 INV7
#define INV15		INV8 INV7
static char *get_opcode_name(cell_t opcode)
{
	char *opnames[] = {
		"nop", INV15
		"call", "jump", "exec", "ret", INV12
		"literal", "lit_load", "lit_push", INV13
		"branch", "zbranch", "=zbranch", INV13
		"?for", "next", "i", "j", INV12
		"/mod", "u/mod", "mul", "umul", INV12
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

static int get_opcode_cellcount(cell_t opcode)
{
	int opcode_cellcount[] = {
		1, INV15
		2, 2, 1, 1, INV12
		2, 2, 1, INV13
		2, 2, 2, INV13
		2, 2, 1, 1, INV12
		1, 1, 1, 1, INV12
		1, 1, 1, 1, 1, 1, 1, 1
	};

	if (opcode <= SHUNT)
		return opcode_cellcount[opcode];
	else 
		return 0;
}

static void disassemble5(cell_t *pc)
{
	int i;

	for (i = 0; i < 5; i++) {
		disassemble(pc);
		printf("\n");
		pc += get_opcode_cellcount(*pc);
	}
	printf("%p:\n", pc);
}

#define LINESTR		line+idx, sizeof(line)-idx
#define CODE_COL	10
#define OPERAND_COL	20
#define COMMENT_COL	40
static void disasm_blanks(void)
{
	printf("%*s", COMMENT_COL, " ");
}

static void disassemble(cell_t *pc)
{
	cell_t opcode;
	char *opstr;
	char line[80];
	int idx;

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
}
#endif /* DEBUG */
