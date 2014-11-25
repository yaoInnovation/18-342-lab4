#include <task.h>


#define NUM_TCB 2

int print_tasks(task_t* tasks  __attribute__((unused)), 
	size_t num_tasks  __attribute__((unused))) {
	int i = 0 ;
	printf("********************Print assigned tasks****************\n");
	for(; i < num_tasks; i++) {
		printf("task no:%d\n", i);
		printf("task function entry po:%x\n", tasks[i].lambda);
		printf("task data:%c\n", tasks[i].data);
		printf("task stack pos:%x\n", tasks[i].stack_pos);
	}
	printf("\n");
	return 0;
}

int print_sys_tcb(tcb_t* system_tcb) {
	int i = 1;
	printf("********************Print System TCB***************\n");
	for(; i<= NUM_TCB; i++) {
		printf("tcb no:%d\n", i);
		printf("tcb cur pos:%x\n", system_tcb[i].context.r4);
		printf("tcb data:%c\n", (char)(system_tcb[i].context.r5));
		printf("task stack pos:%x\n", system_tcb[i].context.r6);
	}
	printf("\n");
	return 0;
}
