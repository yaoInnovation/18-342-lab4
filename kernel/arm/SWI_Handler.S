/** @file SWI_Handler.S
 *
 * 	@brief Wrapper before calling SWI_Handler_C func
 *
 * 	@author Yao Zhou <yaozhou@andrew.cmu.edu>
 *  		Congshan Lv<congshal@andrew.cmu.edu>
 * 	@date   Fri, 07 Nov 2014 23:37:16 -0400
 */
#include <bits/swi.h>

.extern SWI_Handler_C
.global SWI_Handler

SWI_Handler:	
				@ store registers and link register
				STMFD	sp!, {r0-r12,lr}								

				@ recover original jump table from the memory location
				LDR 	r2, =PRESERVED_R8
				LDR		r8, [r2]

				@ Store SPSR
        		MRS   r2, spsr
        		STR   r2, [sp, #100]

        		@ Mask IRQ in CPSR
				MRS		r2,	cpsr			
				AND		r2, r2, #0xFFFFFF7F
				MSR		cpsr, r2

				@ set up parameters for swi
				MOV		r1,	sp			

				@ load swi instruction and get swi number			
				LDR 	r0, [lr,#-4]				
				BIC	r0, r0, #0xff000000				

				@ Store stack and link register
        		STMFD sp, {sp, lr}^
        		ADD   sp, sp, #-8
				
				@ call SWI handler in C
				BL 		SWI_Handler_C
				
				@ Restore stack and link register
				LDMFD sp, {sp, lr}^
        		ADD   sp, sp, #8

				@ Restore spsr
        		LDR   r2, [sp, #100]
        		MSR   spsr, r2

				@ finish swi, recover registers and return
				LDMFD	sp!, {r0-r12,lr}		
				MOVS	pc, lr				
