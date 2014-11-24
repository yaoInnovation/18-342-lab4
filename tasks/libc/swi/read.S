/** @file read.S
 *
 * @brief read sycall wrapper
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 * @date   Wed, 05 Oct 2014 22:08:16 -0400
 * 
 */

#include <asm.h>
#include <bits/swi.h>
#include <errno.h>

	.file "read.S"
	
FUNC(read)
	@ Add code here.  Put your names at the top.
	STMFD	sp!, {r1} 	@ protect register r1
	SWI	READ_SWI		@ call READ syscall 
	CMP	r0, #0 			@ check return value
	BGE	.return			@ if return value is positive, simply return it
	RSB	r0, r0, #0		@ if it is negtive, get error number
	LDR	r1, =errno		@ get error no address
	STR	r0, [r1]		@ store error no back
	MOV r0, #-1			@ set return number to -1

.return:
	LDMFD	sp!, {r1} 	@ recover r1
	MOV 	pc, lr		@ return 
