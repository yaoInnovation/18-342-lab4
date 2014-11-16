/** @file write.S
 *
 * @brief write sycall wrapper
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 07 Oct 2007 01:53:46 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-29
 */

#include <asm.h>
#include <bits/swi.h>

	.file "write.S"

FUNC(write)
	@ Add code here.  Put your names at the top.
	SWI	WRITE_SWI	@ call write syscall
	CMP	r0, #0		@ check return value
	BGE	.return		@ if return positive, simple return
	RSB	r0, r0, #0	@ if it's negative, get error number
	LDR	r1, =errno	@ get error no address
	STR	r0, [r1]	@ store error no back
	MOV	r0, #-1		@ set return no to -1

.return:
	MOV	pc, lr		@ return
