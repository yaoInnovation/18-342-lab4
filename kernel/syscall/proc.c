/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Sat, 22 Nov 2014
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>
#include <../sched/sched_i.h>

#define NULL 0
int task_create(task_t* tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	/* invalid task number */
	if(num_tasks <= 0 || num_tasks > OS_AVAIL_TASKS){
		return -EINVAL;
	}
	/* invalid address */
	if(valid_addr(tasks, num_tasks, USR_START_ADDR, USR_END_ADDR) == 0){
		return -EFAULT;
	}

	disable_interrupts();
	/* run queue init */
	runqueue_init();
	/* dev init */
	dev_init();

	/* task not schedulable */
	if(assign_schedule(&tasks, num_tasks) == 0){
		return -ESCHED;
	}
	/* allocate user-stacks and init the kernel contexts of the given threads*/
	allocate_tasks(&tasks, num_tasks);
	/* allocate stack and init the kernel context of IDLE */
	sched_init(NULL);
	/* dispatch */
	//printf("sched init finished\n");
	dispatch_nosave();
	/* go to dispatch, never should come back */
	return 1;
}

int event_wait(unsigned int dev  __attribute__((unused)))
{
	/* invalid dev */
	if(dev > NUM_DEVICES){
		return -EINVAL;
	}
	/* wait on dev */
	dev_wait(dev);

	return 0;
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
