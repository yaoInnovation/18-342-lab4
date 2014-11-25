/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Fri, 21 Nov 2014 1:13:29
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif
#define NULL 0

mutex_t gtMutex[OS_NUM_MUTEX];
int volatile next_mutex_index = 0;

void mutex_init()
{
	int i;
	for(i = 0; i < OS_NUM_MUTEX; i++) {
		gtMutex[i].bAvailable = TRUE;
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = FALSE;
		gtMutex[i].pSleep_queue = NULL;
	}
}

int mutex_create(void)
{
	disable_interrupts();

	/* no available mutex */
	if(next_mutex_index == OS_NUM_MUTEX){
		enable_interrupts();
		return -ENOMEM;
	}
	/* search through the array to find an available item */
	else{
		int i;
		for( i = 0; i < OS_NUM_MUTEX; i++){
			if(gtMutex[i].bAvailable){
				gtMutex[i].bAvailable = FALSE;
				break;
			}
		}
		/* increase index by one */
		next_mutex_index++;
		enable_interrupts();
		/* return the mutex id */
		return i;
	}
}

int mutex_lock(int mutex  __attribute__((unused)))
{
	disable_interrupts();

	/* invalid mutex id, return error code */
	if(mutex < 0 || mutex >= OS_NUM_MUTEX || gtMutex[mutex].bAvailable){
		enable_interrupts();
		return -EINVAL;
	}
	else{
		tcb_t* cur_tcb = get_cur_tcb();
		/* current task is already holding the lock, return error code */
		if(cur_tcb == gtMutex[mutex].pHolding_Tcb){
			enable_interrupts();
			return -EDEADLOCK;
		}
		/* lock is holding by others, put current task to sleep */
		else if(gtMutex[mutex].bLock){
			/* put the task into sleep queue */
			if(gtMutex[mutex].pSleep_queue == NULL){
				gtMutex[mutex].pSleep_queue = cur_tcb;
				cur_tcb->sleep_queue = NULL;
			}
			else{
				tcb_t* tmp = gtMutex[mutex].pSleep_queue;
				while(tmp->sleep_queue != NULL){
					tmp = tmp->sleep_queue;
				}
				tmp->sleep_queue = cur_tcb;
				cur_tcb->sleep_queue = NULL;
			}
			/* sleep & context switch */
			dispatch_sleep();
		}
		/* no one's holding it, get go! */
		gtMutex[mutex].pHolding_Tcb = cur_tcb;
		gtMutex[mutex].bLock = TRUE;

		cur_tcb->holds_lock ++;
		//cur_tcb->native_prio = cur_tcb->cur_prio;
		//cur_tcb->cur_prio = 0;

		enable_interrupts();
		return 0;
	}
}

int mutex_unlock(int mutex  __attribute__((unused)))
{
	disable_interrupts();

	/* invalid mutex id, return error code */
	if(mutex < 0 || mutex >= OS_NUM_MUTEX || gtMutex[mutex].bAvailable){
		enable_interrupts();
		return -EINVAL;
	}
	else{
		tcb_t* cur_tcb = get_cur_tcb();
		/* current task does not hold the lock, return error code */
		if(cur_tcb != gtMutex[mutex].pHolding_Tcb){
			enable_interrupts();
			return -EPERM;
		}
		else{
			/* release the lock */
			gtMutex[mutex].pHolding_Tcb = NULL;
			gtMutex[mutex].bLock = FALSE;
			/* there is task waiting in the sleep queue, make it able to run */
			if(gtMutex[mutex].pSleep_queue != NULL){
				tcb_t* tmp = gtMutex[mutex].pSleep_queue;
				gtMutex[mutex].pSleep_queue = tmp->sleep_queue;
				tmp->sleep_queue = NULL;

					runqueue_add(tmp, tmp->cur_prio);
			}
			/* decrease the num of locks hold by current task by one */
			if(cur_tcb->holds_lock != 0)
				cur_tcb->holds_lock --;
			/* if current task does not hold any other locks, set priority back */
		/*	if(cur_tcb->holds_lock == 0){
				cur_tcb->cur_prio = cur_tcb->native_prio;
			} */

			enable_interrupts();
			return 0;

		}
	}
}
