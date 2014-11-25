/** @file dagger.c
 *
 * @brief Creates two simple periodic tasks.
 *
 * @note  This is like knife -- but smaller :)
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-30
 */
#include <stdio.h>
#include <task.h>
#include <unistd.h>
#include <lock.h>

volatile char S = 'a';
volatile int mutex = -1;

void panic(const char* str)
{
	puts(str);
	while(1);
}

void fun1()
{
	mutex = mutex_create();
	while(1)
	{
		printf("Task 1:requset mutex\n");
		mutex_lock(mutex);
		printf("Task 1:Enter Critical Section\n");
		mutex_unlock(mutex);
		printf("Task 1:release mutex\n");

		if (event_wait(0) < 0)
			panic("Dev 0 failed");
	}
}

void fun2()
{
	while(1)
	{
		printf("Task 2:request mutex\n");
		mutex_lock(mutex);
		printf("Task 2:Enter Critical Section\n");
		sleep(10000);
		mutex_unlock(mutex);
		printf("Task 2:release mutex\n");
		if (event_wait(1) < 0)
			panic("Dev 1 failed");
	}
}

int main(int argc, char** argv)
{
	task_t tasks[2];
	tasks[0].lambda = fun1;
	tasks[0].data = 0;
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 1;
	tasks[0].T = PERIOD_DEV0;
	tasks[1].lambda = fun2;
	tasks[1].data = 0;
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 1;
	tasks[1].T = PERIOD_DEV1;

	task_create(tasks, 2);

	argc=argc; /* remove compiler warning */
	argv[0]=argv[0]; /* remove compiler warning */

	puts("Why did your code get here!\n");
	return 0;
}
