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

static int     hold_lit  = 0;
static float_t hold_flit = 0;

static int is_func_pcd(cell_t target)
{
	code_t *pc;
	pc = (code_t *) target;
	return (pc >= &pcd0[0]) && (pc < &pcd0[PCD_SIZE / sizeof(*pcd0)]);
}

static void call(cell_t target)
{
	forth_func_t func = (forth_func_t) target;

	func();
}

void execute(cell_t target)
{
	if (is_func_pcd(target)) {
		interpreter(target);
	} else {
		call(target);
	}
}

static cell_t *op_jump(cell_t pc)
{
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

static cell_t *op_brk(cell_t *pc)
{
	RPUSH((cell_t) pc);
#if 0
	/*
	 * To be enabled when the debugger is checked in
	 */
	mu_dbg();
#endif /* 0 */
	pc = (cell_t *) RPOP;

	return pc;
}

static cell_t *op_lit(cell_t *pc)
{
	hold_lit = *pc++;
	return pc;
}

static cell_t *op_flit(cell_t *pc)
{
	float_t *p;

	p = (float_t *) pc;
	bcopy(pc, &hold_flit, sizeof(hold_flit));
	
	pc = (cell_t *) (p+1);

	return pc;
}

static void interpreter(cell_t codep)
{
	cell_t  *pc;

	pc = (cell_t *) codep;

	/*
	 * We exit the interpreter when we return to 0
	 */
	RPUSH(0);

	do {
		switch(*pc++) {
		case BRK: pc = op_brk(pc-1);			break;
		case NOP:					break;

		case JUMP: pc = op_jump(*pc);			break;
		case EXEC: pc = op_jump(POP); 			break;
		case CALL: RPUSH(pc +1); pc = op_jump(*pc);	break;
		case RET:  pc = (cell_t *) RPOP;		break;

		case LITERAL:  pc = op_lit(pc); PUSH(hold_lit);	break;
		case LIT_LOAD: pc = op_lit(pc);			break;
		case LIT_PUSH: PUSH(hold_lit);			break;

		case FLITERAL:  pc = op_flit(pc); fpush(hold_flit); break;
		case FLIT_LOAD: pc = op_flit(pc);		break;
		case FLIT_PUSH: fpush(hold_flit);		break;

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
			pc = op_brk(pc);
			break;
		}
	} while (pc);
}

