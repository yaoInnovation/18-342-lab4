/** @file setup.S
 *
 *  @brief  set up enviroment for user program.
 *						
 *	
 * 	@author Yao Zhou <yaozhou@andrew.cmu.edu>
 *  		Congshan Lv<congshal@andrew.cmu.edu>
 * 	@date   Fri, 07 Nov 2014 23:37:16 -0400
 */

.global setup

.comm PRESERVED_SP, 4, 4	@ allocate a space to store original stack
.comm PRESERVED_R8, 4, 4	@ allocate a space to store original jump table

.equ IRQ_STACK, 0xA2000000
.equ USER_STACK, 0xA3000000
.equ USER_PROM_ADDR, 0xA0000000

#include <bits/swi.h>

setup:
	STMFD sp!, {r1-r7, r9-r12, lr} @ store bank registers and link regsiter
	LDR r2, =PRESERVED_SP	@ store stack address
	STR sp, [r2]
	LDR r2, =PRESERVED_R8	@ store original jump table
	STR r8, [r2]
	
	MRS r2, cpsr		@ create IRQ mode stack
	AND r2, r2, 		#0xFFFFFFF0	@ get last 4 bits
	ORR   r2, r2, 		#0x2	@ 10010 - IRQ mode
    MSR   cpsr, r2 		@ switch IRQ mode
    MOV   sp, 			#0xA2000000	@ create stack for IRQ mode
	
	MRS	r2, cpsr		@ get current cpsr
	AND     r2, r2, 	#0xFFFFFF00	@ get last 8 bits
	ORR	r2, r2, #0x50	@ mask FIQ and enter user mode 10000.
	MSR	cpsr, r2   		@ switch to USER mode with FIQ masked
	MOV sp, #0xA3000000	@ set up stack address to create a descending user stack

	MOV   r2, #0   	@ push null to the argv
	STMFD sp!, {r2} 

	MOV r2, r0		@ load U-Boot argc
	
.push_argv:
	SUBS r2, r2, #1   	@ get the number of rest arguments
	LDRGE r3, [r1, r2, LSL #2] @ load parameter to register
	STMGEFD sp!, {r3}	@ push it into user stack
	BGE .push_argv		@ if not finished, jump back to .push_argv

.push_argc:
	MOV r2, r0
	STMFD sp!, {r0}		@ push argc to user stack
			
.load_prog:		
	MOV	r2, #0xA0000000		@ get the address of loaded user program
	BLX 	r2   			@ jum to loaded user program	
	@SWI 	EXIT_SWI		@ exit user program, call sys call exit

