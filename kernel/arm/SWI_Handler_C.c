/** @file SWI_Handler_C.c
 *
 *      @brief SWI Handler in C
 *
 *      @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *              Congshan Lv<congshal@andrew.cmu.edu>
 *      @date   Fri, 07 Nov 2014 23:37:16 -0400
 */

#include <exports.h>
#include <bits/swi.h>
#include <lock.h>
#include <syscall.h>

void SWI_Handler_C(unsigned swi_no, unsigned* regs){

        char* buf = 0;    //a simple pointer
        //int exit_no = -1; //used to store exit code
        int fd = -1;      //file descriptor
        int bytes = 0;    //store the return number from read() or write()
        unsigned long m_sec;
        task_t* task = NULL;
        size_t num = 0;
        int mutex = -1;
        int dev = -1;

        switch(swi_no) {
                /*case EXIT_SWI: {
                        // get exit code
                        exit_no = regs[0];
                        // exit!
                        exit(exit_no);
                }*/
                case READ_SWI: {
                        // get parameters
                        fd = regs[0];
                        buf = (char*)regs[1];
                        bytes = regs[2];
                        // call my read function
                        regs[0] = read_syscall(fd,buf,bytes);
                        break;
                }
                case WRITE_SWI: {
                        // get parameters
                        fd = regs[0];
                        buf = (char*)regs[1];
                        bytes = regs[2];
                        // call my write function
                        regs[0] = write_syscall(fd,buf,bytes);
			break; 
		}
                case TIME_SWI: {
                        // call my time function
                        regs[0] = time_syscall();
                        break;
                }
                case SLEEP_SWI: {
                        // get parameter
                        m_sec = regs[0];
                        // call my sleep function
                        sleep_syscall(m_sec);
                        break;
                }
                case CREATE_SWI: {
                        task = (task_t*) regs[0];
                        num = (size_t) regs[1];

                        regs[0] = task_create(task, num);
                        return;
                }
                case MUTEX_CREATE: {
                        regs[0] = mutex_create();
                }
                case MUTEX_LOCK: {
                        mutex = regs[0];

                        regs[0] = mutex_lock(mutex);
                        return;
                }
                case MUTEX_UNLOCK: {
                        mutex = regs[0];

                        regs[0] = mutex_unlock(mutex);
                        return;
                }
                case EVENT_WAIT: {
                        dev = regs[0];

                        regs[0] = event_wait(dev);
                        return;
                }
		default: {
                        // unimplemented SWI or invalid SWI number
                        printf("Invalid SWI number: %d\n", swi_no);
                        regs[0] = 0x0badc0de;
                }
        }
        return;
}
