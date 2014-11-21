/** @file crt0.S
 *
 * @brief Execution startup routine
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Thur, 20 Nov 2014 11:30:39 -0400
 */

#include <asm.h>
#include <bits/swi.h>

	.file "crt0.S"

FUNC(_start)
	@ needs code.  Put your names at the top.
	LDR     r0, [sp]	@ Load argc from stack into r0
	ADD     r1, sp, #4	@ Load argv address (sp+4) into r1
	BL      main		@ Branch into main function

	@ if main returns, go to an infinite loop	
.InfiniteLoop:
	B	.InfiniteLoop

