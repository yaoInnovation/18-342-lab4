/** @file main.c
 *
 * @brief kernel main
 *
 * @author 
 *	   
 *	   
 * @date   
 */
 
#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <types.h>

#define SWI_ADDR 0x08
#define IRQ_ADDR 0x18
#define TIME_STEP 10
#define TIME_COUNTER_STEP 32500

extern int install_MyHandler(unsigned new_addr, unsigned* vec_addr,
        unsigned* instr1, unsigned* instr2);
extern int recover_MyHandler(unsigned *vec_addr, unsigned instr1, 
        unsigned instr2);
extern void SWI_Handler_C(unsigned swi_no, unsigned* regs);
extern void IRQ_Handler_C();
extern unsigned SWI_Handler();
extern void irq_wrapper();
extern int setup(int argc, char* argv[]);

extern ssize_t read(int fd, char* buf, size_t count);
extern ssize_t write(int fd, char* buf, size_t count);
extern void sleep(unsigned long time);
extern unsigned long time();

uint32_t global_data;

volatile unsigned long sys_time;
volatile unsigned long time_drift;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{

	app_startup();
	global_data = table;
	/* add your code up to assert statement */

	
	// Address of the old SWI Handler
	unsigned* swi_vec_addr = (unsigned*)(SWI_ADDR);
	// Address of my SWI Handler
	unsigned  new_swi_addr = (unsigned)&SWI_Handler;
	// Address of the old IRQ Handler
	unsigned* irq_vec_addr = (unsigned*)(IRQ_ADDR);
	// Address of my IRQ Handler
	unsigned  new_irq_addr = (unsigned)&IRQ_Handler;
	
	// Save the first two instrs of the old SWI Handler
	unsigned swi_instr1, swi_instr2;
	// Save the first two instrs of the old IRQ Handler
	unsigned irq_instr1, irq_instr2;
	
	// Install My SWI Handler
	install_MyHandler(new_swi_addr, swi_vec_addr, &swi_instr1, &swi_instr2);
	// Install My IRQ Handler
	install_MyHandler(new_irq_addr, irq_vec_addr, &irq_instr1, &irq_instr2);
	
	// Initialize sys_time
	sys_time = 0;
	time_drift = 0;
	
	// Save regs
	unsigned ICMR, ICLR, OIER;
	ICMR = reg_read(INT_ICMR_ADDR);
	ICLR = reg_read(INT_ICLR_ADDR);
	OIER = reg_read(OSTMR_OIER_ADDR);

	// Set regs
	reg_write(INT_ICMR_ADDR, 1<<INT_OSTMR_0);//Only enable OSTMR0
	reg_write(INT_ICLR_ADDR, 0x0);//Set all interrupts from OSTMR0 to be IRQ
	
	reg_clear(OSTMR_OIER_ADDR, OSTMR_OIER_E1|OSTMR_OIER_E2|OSTMR_OIER_E3);
	reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
	reg_write(OSTMR_OSMR_ADDR(0), reg_read(OSTMR_OSCR_ADDR)+TIME_COUNTER_STEP);
	
	mutex_init();

	// Change mode, jump to usr program
	setup(argc, argv);
	
	/*// Restore regs
	reg_write(INT_ICMR_ADDR, ICMR);
	reg_write(INT_ICLR_ADDR, ICLR);
	reg_write(OSTMR_OIER_ADDR, OIER);

	// Recover old SWI Handler
	recover_MyHandler(swi_vec_addr, swi_instr1, swi_instr2);
	// Recover old IRQ Handler
	recover_MyHandler(irq_vec_addr, irq_instr1, irq_instr2);*/

	assert(0);        /* should never get here */
}
