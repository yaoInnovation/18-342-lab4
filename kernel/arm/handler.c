/** @file handler.c
 *
 *  @brief This file contains hijack handler and recover original handler
 *
 *  @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *          Congshan Lv<congshal@andrew.cmu.edu>
 *  @date   Fri, 07 Nov 2014 23:37:16 -0400
 */

#include <exports.h>

int install_MyHandler(unsigned new_addr, unsigned* vec_addr, 
	unsigned* instr1, unsigned* instr2){

	unsigned vec_instr, op, sign, offset, handler_addr;
	
	// Get SWI Handler instrucntion in the vector table
	// To get the 24th bit:U bit which shows up or down / direciton
	vec_instr = *vec_addr;
	sign = vec_instr >> 23 & 1;

	// Get the operator part of LDR instruction
	op = vec_instr & 0xFFFFF000;

	// Check whether instruction at 0x08 is in format of LDR pc, [pc, #offset]
	// 0xE59FF000 means up and 0xE51FF000 means down (1001 up,1000 down)
	if(op != (unsigned)0xE59FF000 && op != (unsigned)0xE51FF000) {
		printf("Unrecognized Instruction\n");
		return 0x0BADC0E;
	}

	// Get the offset part of the LDR instruction
	offset = vec_instr & 0xFFF;
	if( sign == 0) {
		offset = offset * -1;
	}

	// get the address of handler
	handler_addr = *(unsigned *)((unsigned)vec_addr + offset + 0x8);

	// store the original two instructions for recovery
	*instr1 = *(unsigned *)handler_addr;
	*instr2 = *(unsigned *)(handler_addr + 4);

	// install my handler
	// first instuction: load pc, [pc,#-4] @ ldr the address in the next 4 bytes into pc
	// change that address to my handler
	*(unsigned*)handler_addr = 0xE51FF004;
	*(unsigned*)(handler_addr + 4) = new_addr;
	// now, after pc gets to original swi_handler, the first two instruction will lead to my swi_handler
	return 0;
}

int recover_MyHandler(unsigned *vec_addr, unsigned instr1, unsigned instr2){
	unsigned vec_instr, sign, offset, handler_addr;

	// Get the handler instrucntion in the vector table
	vec_instr = *vec_addr;
	sign = vec_instr >> 23 & 1;

	// Get the offset part of the LDR instruction
	offset = vec_instr & 0xFFF;
	if( sign == 0) {
		offset = offset * -1;
	}

	// Get the address of the handler
	handler_addr = *(unsigned *)((unsigned)vec_addr + offset + 0x8);
	
	// Recover instructions
	*(unsigned*)handler_addr = instr1;
	*(unsigned*)(handler_addr + 4) = instr2;

	return 0;
}
