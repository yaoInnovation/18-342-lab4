/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "mutex_create.S"

FUNC(mutex_create)
	    @ Add code here.  Put your name at the top.
        STMFD   sp!, {r1}       @ protect register r1
       	SWI     MUTEX_CREATE    @ call mutex create syscall
        CMP     r0, #0          @ check return value
        BGE     .return         @ if return value is positive, simply return it
        RSB     r0, r0, #0      @ if it is negtive, get error number
        LDR     r1, =errno      @ get error no address
        STR     r0, [r1]        @ store error no back
        MOV     r0, #-1         @ set return number to -1

.return:
        LDMFD   sp!, {r1}       @ recover r1
        MOV     pc, lr          @ return                              
