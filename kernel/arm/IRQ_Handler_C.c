/** @file IRQ_Handler_C.c
 *
 * 	@brief IRQ Handler in C
 *
 * 	@author Yao Zhou <yaozhou@andrew.cmu.edu>
 *  		Congshan Lv<congshal@andrew.cmu.edu>
 * 	@date   Fri, 07 Nov 2014 23:37:16 -0400
 */

//#include "main.h"
#include <exports.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <bits/fileno.h>
#include <bits/errno.h>
#include <bits/swi.h>
#include <config.h>
#include <syscall.h>
#include <device.h>

extern volatile unsigned long sys_time;
extern volatile unsigned long time_drift;

void IRQ_Handler_C(){
	//Check whether a timer interrupt has occurred
	if((reg_read(OSTMR_OSSR_ADDR)&OSTMR_OSSR_M0)
	 &&(reg_read(INT_ICPR_ADDR)>>INT_OSTMR_0)){

      // Increment time by 10ms
      sys_time += TIME_STEP;

  	  // Reset time clock, avoid accumulation.
      reg_write(OSTMR_OSCR_ADDR, 0X00);

      // Set OSMR to next match point (10ms later)
      reg_write(OSTMR_OSMR_ADDR(0), reg_read(OSTMR_OSCR_ADDR) + TIME_COUNTER_STEP);

      // reset match register match bit
      reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);

      // update devices to awake sleeping programs.
	  dev_update(sys_time);
	}
	return;
}
