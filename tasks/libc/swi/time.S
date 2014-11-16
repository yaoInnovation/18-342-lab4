/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 * @date 2014-11-5
 */

#include <asm.h>
#include <bits/swi.h>

	.file "time.S"

FUNC(time)
	@ Add code here.  Put your name at the top.
	@ no need to protect registers.
	SWI TIME_SWI		@ time syscall
	MOV	pc, lr			@ return

