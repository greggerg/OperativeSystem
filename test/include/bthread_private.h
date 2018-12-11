#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)
#include "tqueue.h"
#include <stdint.h>
typedef struct TQueueNode {
        struct TQueueNode * next;
        void * data;
    }TQueueNode;
//bthread structure
 typedef struct {
//thread identifier
	bthread_t tid;
//the body routine
	bthread_routine body;
//body argument
	void* arg;
//execution state
	bthread_state state;
//attributes	
	bthread_attr_t attr;
//stack 
	char* stack;
//stack context
	jmp_buf context;
//return value	
void* retval;
   } __bthread_private;

//scheduler
 typedef struct {
	//list of threads
	TQueue queue;
	//currently executing thread
	TQueue current_item;
	jmp_buf context;
	//used to generate unique thread identifiers
	bthread_t current_tid;
  } __bthread_scheduler_private;

static __bthread_scheduler_private* sp;
/*This private function creates, maintains and returns a static pointer to the singleton instance of
__bthread_scheduler_private. Fields of this structure need to be initialized as NULL.
Other functions will call this method to obtain this pointer. This function should not be accessible
outside the library*/

__bthread_scheduler_private* bthread_get_scheduler(){
if(sp==NULL) {
printf("Creating the scheduler\n");
sp =  malloc(sizeof(__bthread_scheduler_private));
sp->queue= NULL;
sp->current_item=NULL;
//sp->context;
sp->current_tid=-1;
}
return sp;
};

void bthread_cleanup(){
free(sp);
};
static TQueue bthread_get_queue_at(bthread_t bthread){
sp = bthread_get_scheduler();	
return tqueue_at_offset(sp->queue,bthread);
}

/*  and; finally the function returns 1.*/
static int bthread_check_if_zombie(bthread_t bthread, void **retval){	
TQueue queue = bthread_get_queue_at(bthread);
if(queue==NULL){
printf("Queue not present\n");
return -1;
}else{
__bthread_private* tp = malloc(sizeof(__bthread_private));
printf("Thread structure created \n");
tp = (__bthread_private*)tqueue_get_data(queue);
	//Checks whether the thread referenced by the parameter bthread has reached a zombie state.
printf("Thread structure created \n");
	if(tp->state==__BTHREAD_ZOMBIE){
	printf("Zombie\n");
	   if(retval!=NULL){
			//if retval is not NULL the exit status of the target thread (i.e. the value that was supplied to bthread_exit) is copied into the location pointed to by *retval;
			tp->retval=*retval;
			//the thread's stack is freed
			free(tp->stack);
			//the thread's private data structure is removed from the queue (Note: depending on your implementation, you might need to pay attention to 
			//the special case where the scheduler's queue pointer itself changes!)
			//tqueue_pop(queue);
		}
		return 1;
	   }else{
		    //If it's not the case the function returns 0.	
		    return 0;
		}

}
}
