/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Fri, 21 Nov 2014 1:59:41
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static __attribute__((unused)) tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle __attribute__((unused)))
{
	/* set current task to idle */
	cur_tcb = idle;
	/* remove idle from run queue */
	runqueue_remove(idle->cur_prio);
	/* switch to idle */
	ctx_switch_half(&idle->context);
}


/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	/* current task has the highest priority */
	if(cur_tcb->cur_prio <= highest_prio())
		return;
	else{
		/* add current task to run queue*/
		runqueue_add(cur_tcb, cur_tcb->cur_prio);

		/* context switch */
		tcb_t* former_tcb = cur_tcb;
		cur_tcb = runqueue_remove(highest_prio());

		ctx_switch_full(&cur_tcb->context, &former_tcb->context);
	}
}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	/* simply switch to the runnable task with highest priority */
	cur_tcb = runqueue_remove(highest_prio());
	printf("remove highest priority(finished)\n");
	ctx_switch_half(&cur_tcb->context);
}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	tcb_t* former_tcb = cur_tcb;
	cur_tcb = runqueue_remove(highest_prio());

	ctx_switch_full(&cur_tcb->context, &former_tcb->context);
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	return cur_tcb->cur_prio;	
}
/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb;
}
