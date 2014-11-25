/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Fri, 21 Nov 2014 23:24:54
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include <syscall.h>
#include <arm/reg.h>

extern volatile unsigned long sys_time;
extern volatile unsigned long time_drift;

unsigned long time_syscall(void)
{
	// Read the OSCR register
	volatile unsigned long oscr = reg_read(OSTMR_OSCR_ADDR);
	// Calculate the current sys_time
	unsigned long time = sys_time + 
		TIME_STEP*((oscr+time_drift)/TIME_COUNTER_STEP);
	return time;
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis  __attribute__((unused)))
{
	// compute the end time stamp
	unsigned long up_time = time_syscall() + millis;
	// keep waiting for a certain duration
	while ( time_syscall()< up_time){
	};

	return;
}
