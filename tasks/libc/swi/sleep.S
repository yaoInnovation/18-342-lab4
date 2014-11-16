/** @file sleep.S
 *
 * @brief sleep sycall wrapper
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 * @date 2014-11-5
 */

#include <asm.h>
#include <bits/swi.h>

	.file "sleep.S"

FUNC(sleep)
	@ Add code here.  Put your names at the top.
	@ no need to protect registers.
	SWI SLEEP_SWI		@ sleep syscall
	MOV	pc, lr			@ return
