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
 * 00 - NOP
 *
 * 10 - CALL (Target address follows instruction)
 * 11 - JUMP (Target address follows instruction)
 * 12 - Execute (Target address is on the data stack)
 * 13 - RET (Return from CALL)
 *
 * 20 - Push Literal (data follows instruction)
 * 21 - Load Literal (into a holding buffer, data follows instruction)
 * 22 - Push Held Literal (from the holding buffer)
 *
 * 30 - Unconditional Branch
 * 31 - Destructive Branch If Zero, drop top element
 * 32 - Non-destructive Branch If Zero, do not modify stack
 * 
 * 40 - For
 * 41 - Next
 * 42 - I (push current for loop index)
 * 43 - J (push outer loop index within same word)
 *
 * 60 - Push to R stack
 * 61 - Pop from R stack
 * 62 - Copy from R stack
 *
 * Note: The R stack is -not- the host environments return
 * stack.  It is a stack different from the data stack.
 */
#ifndef __MUFORTH_TOKEN_H__
#define __MUFORTH_TOKEN_H__

typedef enum opcode_e opcode_t;
enum opcode_e {
	NOP,

	CALL = 0x10,
	JUMP,
	EXEC,
	RET,

	LITERAL = 0x20,
	LIT_LOAD,
	LIT_PUSH,

	BRANCH = 0x30,
	ZBRANCH,
	_ZBRANCH,

	QFOR = 0x40,
	NEXT,
	I,
	J,

	DROP = 0x60,
	DROP2,
	PUSH_TO_R,
	PUSH2_TO_R,
	POP_FROM_R,
	POP2_FROM_R,
	COPY_FROM_R,
	SHUNT,
};

#endif /* __MUFORTH_TOKEN_H__ */
