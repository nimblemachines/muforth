/*
 * opcode.c
 *
 * This implements an opcode compiler and interpreter.
 *
 * This file was created as part of the transition of muforth to
 * support generic processors.
 *
 */
#include <stdio.h>
#include <assert.h>
#include "opcode.h"
#include "muforth.h"

/*
 * op_compile(opcode_t)
 * 
 * This function will lay an opcode down in the dictionary.
 */
static void op_compile(cell_t op)
{
	*pcd++ = op;
}

static void fcompile(float_t f)
{
	float_t *p;

	p = (float_t *) pcd;
	bcopy(&f, p, sizeof(f));
	pcd = (code_t *) (p+1);
}

/*
 * mu_resolve()
 * 
 * resolve a forward or backward jump. In this usage "src" is the addres
 * just _past_ the branch we need to fix up.
 *
 * : resolve>  ( src dest)  resolve ;
 */
void mu_resolve(void)
{
	cell_t *src, dest;
	
	dest = POP;
	src = (cell_t *) POP;
		
	src[-1] = dest;
}

void mu_compile_exit(void)
{
	cell_t *pc;

	pc = (cell_t *) pcd;
	if (pc[-2] == CALL)
		pc[-2] = JUMP;

	op_compile(RET);
}

#define COMPILE(name, opcode)	\
void mu_compile_##name(void) {	\
	op_compile(opcode);	\
}
#define COMPILE_POP(name, opcode)	\
void mu_compile_##name(void) {	\
	op_compile(opcode);	\
	op_compile(POP);	\
}
#define COMPILE_FPOP(name, opcode)	\
void mu_compile_##name(void) {	\
	op_compile(opcode);	\
	fcompile(fpop());	\
}
#define BRANCH(name, opcode)	\
void mu_compile_##name(void) {	\
	op_compile(opcode);	\
	op_compile(0);		\
	PUSH(pcd);		\
}

COMPILE(push_to_r, PUSH_TO_R);
COMPILE(pop_from_r, POP_FROM_R);
COMPILE(2push_to_r, PUSH2_TO_R);
COMPILE(2pop_from_r, POP2_FROM_R);
COMPILE(copy_from_r, COPY_FROM_R);
COMPILE(drop, DROP);
COMPILE(2drop, DROP2);
COMPILE(i, I);
COMPILE(j, J);
COMPILE(shunt, SHUNT);
COMPILE(literal_push, LIT_PUSH);
COMPILE(execute, EXEC);
COMPILE_POP(literal_load, LIT_LOAD);
COMPILE_POP(call, CALL);

COMPILE_FPOP(fliteral_load, FLIT_LOAD);
COMPILE(fliteral_push, FLIT_PUSH);

BRANCH(branch, BRANCH);
BRANCH(destructive_zbranch, ZBRANCH);
BRANCH(nondestructive_zbranch, _ZBRANCH);
BRANCH(next, NEXT);

void mu_compile_qfor(void)
{
	op_compile(QFOR);
	op_compile(0);
	PUSH(pcd);	/* The address for resolve */
	PUSH(pcd);	/* The address for the loop */
}

