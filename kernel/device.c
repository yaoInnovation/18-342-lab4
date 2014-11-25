/**
 * @file device.c
 *
 * @brief Implements simulated devices.
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-12-01
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Sat, 22 Nov 2014
 */

#include <types.h>
#include <assert.h>

#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <syscall.h>

/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent 
 * tasks with logical devices. 
 * These logical devices should be signalled periodically 
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update 
 * on every timer interrupt. In dev_update check if it is 
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

extern volatile unsigned long sys_time;

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};
static dev_t devices[NUM_DEVICES];

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(void)
{
	int i;
	for(i = 0; i < NUM_DEVICES; i++){
		/* sleep queue init */
		devices[i].sleep_queue = NULL;
		/* match value init */
		devices[i].next_match = sys_time + dev_freq[i];
	}
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev __attribute__((unused)))
{
	//disable_interrupts();

	tcb_t* tmp = devices[dev].sleep_queue;
	if (tmp == NULL) {
		devices[dev].sleep_queue = get_cur_tcb();
	} else {
		while(tmp->sleep_queue != NULL){
			tmp = tmp->sleep_queue;
		}
		tmp->sleep_queue = get_cur_tcb();
		tmp->sleep_queue->sleep_queue = NULL;
	}

	//enable_interrupts();
	dispatch_sleep();
}


/**
 * @brief Signals the occurrence of an event on all applicable devices. 
 * This function should be called on timer interrupts to determine that 
 * the interrupt corresponds to the event frequency of a device. If the 
 * interrupt corresponded to the interrupt frequency of a device, this 
 * function should ensure that the task is made ready to run 
 */
void dev_update(unsigned long millis __attribute__((unused)))
{
	//printf("entering dev_update\n");
	//disable_interrupts();
	int i;
	int flag = 0;
	for(i = 0; i < NUM_DEVICES; i++){
		//printf("next match=%ld,millis=%ld\n",devices[i].next_match,millis);
		/* millis matches a device next_match */
		if(devices[i].next_match <= millis){
			/* make tasks in its sleep queue runnable */
			tcb_t* tmp = devices[i].sleep_queue;
			tcb_t* next;
			while(tmp != NULL){
				flag = 1;
				next = tmp->sleep_queue;
				tmp->sleep_queue = NULL;
				runqueue_add(tmp, tmp->cur_prio);
				tmp = next;
			}
			devices[i].sleep_queue = NULL;

			/* update mext match */
			devices[i].next_match = devices[i].next_match + dev_freq[i];//or = next_match + dev_freq[i] ?		
		}
	}
	//enable_interrupts();
	if(flag == 1)
		dispatch_save();
}

