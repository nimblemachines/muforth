/*
 * token.h
 *
 * The muforth compiler was replaced with an interpreter in
 * July of 2004.  The goal of this transition was to create
 * a muforth environment that could run on all processor
 * families.  Then, one would build, in forth, a meta-compiler.
 * The interpreter is builds a stream of simple byte-sized
 * opcodes with, in some cases, in-lined data.
 *
 * The opcodes supported are:
 *
 * NOP
 *
 * CALL (Target address follows instruction)
 * JUMP (Target address follows instruction)
 * Execute (Target address is on the data stack)
 * RET (Return from CALL)
 *
 * Push Literal (data follows instruction)
 * Load Literal (into a holding buffer, data follows instruction)
 * Push Held Literal (from the holding buffer)
 *
 * Unconditional Branch
 * Destructive Branch If Zero, drop top element
 * Non-destructive Branch If Zero, do not modify stack
 * 
 * For
 * Next
 * I (push current for loop index)
 * J (push outer loop index within same word)
 *
 * Push to R stack
 * Pop from R stack
 * Copy from R stack
 *
 * Note: The R stack is -not- the host environments return
 * stack.  It is a stack different from the data stack.
 */
#ifndef __MUFORTH_TOKEN_H__
#define __MUFORTH_TOKEN_H__

typedef enum opcode_e opcode_t;
enum opcode_e {
	BRK,
	NOP,

	JUMP,
	CALL,
	EXEC,
	RET,

	LITERAL,
	LIT_LOAD,
	LIT_PUSH,

	FLITERAL,
	FLIT_LOAD,
	FLIT_PUSH,

	BRANCH,
	ZBRANCH,
	_ZBRANCH,

	QFOR,
	NEXT,
	I,
	J,

	DROP,
	DROP2,
	PUSH_TO_R,
	PUSH2_TO_R,
	POP_FROM_R,
	POP2_FROM_R,
	COPY_FROM_R,
	SHUNT,
};

#endif /* __MUFORTH_TOKEN_H__ */
