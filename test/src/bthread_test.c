#include <stdio.h>
#include <stdlib.h>
#include "bthread.h"

typedef struct TQueueNode {
        struct TQueueNode * next;
        void * data;
    }
TQueueNode;

void* routine(void *arg){
printf("Hello thread %d\n",*(int *)arg);

return arg;
}

void bthread_test(){
bthread_t bthread = 17;
void *arg = "Greg";
//create
//void *(*start_routine) (void *)
printf("Trying\n");
int result  = bthread_create(&bthread, NULL,&routine, arg);
//start
	//volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
//join
void* retval="42s";
printf("Trying to join\n");
bthread_join(17,&retval);


//yield
//exit
printf("bthread test succeded\n");
}
