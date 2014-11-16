/** @file crt0.S
 *
 * @brief Execution startup routine
 *
 * @author yao zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Thur, 05 Nov 2014 21:55:29 -0400
 */

#include <asm.h>
#include <bits/swi.h>

	.file "crt0.S"

FUNC(_start)
	@ needs code.  Put your names at the top.
	LDR     r0, [sp]	@ Load argc from stack into r0
	ADD     r1, sp, #4	@ Load argv address (sp+4) into r1
	BL      main		@ Branch into main function

	@ user programs(tasks) never exit	
