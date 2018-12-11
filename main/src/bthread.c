#include <stdio.h>
#include <stdlib.h>
#include "../include/bthread.h"
#include "../include/bthread_private.h"
#include "../include/tqueue.h"
#include <stdint.h>
//disable notify for save_context
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#define STACK_SIZE 8

__bthread_scheduler_private *bthread_get_scheduler() {
    if (sp == NULL) {
        printf("Creating the scheduler\n");
        sp = malloc(sizeof(__bthread_scheduler_private));
        sp->queue = NULL;
        sp->current_item = NULL;
        sp->current_tid = 0;
    }
    return sp;
};

/*
Creates a new thread structure and puts it at the end of the queue. The thread identifier (stored in the buffer pointed by bthread) corresponds to the position in the queue.
 The thread is not started when calling this function. Attributes passed through the attr argument are ignored
(thus it is possible to pass a NULL pointer). The stack pointer for new created threads is NULL.*/
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    printf("Position in the queue %ld\n", *bthread);
    __bthread_private *thread = malloc(sizeof(__bthread_private));
    thread->tid = *bthread;
    thread->body = start_routine;
    thread->arg = arg;
    thread->state = __BTHREAD_BLOCKED;
    if (attr != NULL) {
        thread->attr = *attr;
    }
    thread->stack = NULL;

    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    thread->tid = tqueue_enqueue(&scheduler->queue, thread);
    *bthread = thread->tid;
    scheduler->current_item = scheduler->queue;
    return 0;
}


/*Saves the thread context and restores (long-jumps to) the scheduler context. Saving the thread
context is achieved using sigsetjmp, which is similar to setjmp but can also save the signal
mask if the provided additional parameter is not zero (to restore both the context and the signal
mask the corresponding call is siglongjmp). Saving and restoring the signal mask is required
for implementing preemption.*/
void bthread_yield() {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *currentThread = (__bthread_private *) scheduler->current_item;
    if (!save_context(currentThread->context))
        restore_context(scheduler->context);
}


int bthread_join(bthread_t bthread, void **retval) {

    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *thread = (__bthread_private *) tqueue_get_data(scheduler->queue);
    if (thread == NULL) { printf("Null queue\n"); }
    else {
        printf("Thread id = %ld \n", thread->tid);
        thread->state = 2;
        printf("Thread state = %d \n", thread->state);


        printf("Thread body = %p \n", thread->body);
    }
//
    scheduler->current_item = scheduler->queue;

    if (scheduler->current_item == NULL) { printf("Null item\n"); }
    else {
        thread = tqueue_get_data(scheduler->queue);
    }
    save_context(scheduler->context);

    if (bthread_check_if_zombie(bthread, retval)) return 0;
    printf("Not a zombie \n");
    __bthread_private *tp;
    do {
//move to the next one
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private *) tqueue_get_data(scheduler->current_item);
    } while (tp->state != __BTHREAD_READY);
    printf("Restore \n");
    // Restore context or setup stack and perform first call
    if (tp->stack) {
        restore_context(tp->context);
    } else {
        tp->stack = (char *) malloc(sizeof(char) * STACK_SIZE);
#if __x86_64__
        asm __volatile__("movq %0, %%rsp"::
        "r"((intptr_t) (tp->stack + STACK_SIZE - 1)));
#else
        asm __volatile__("movl %0, %%esp" ::
           "r"((intptr_t) (tp->stack + STACK_SIZE - 1)));
#endif
        bthread_exit(tp->body(tp->arg));

    }
}

/*Terminates the calling thread and returns a value via retval that will be available to another
thread in the same process that calls bthread_join, then yields to the scheduler. Between
bthread_exit and the corresponding bthread_join the thread stays in the __BTHREAD_ZOMBIE state.*/
void bthread_exit(void *retval) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *currentThread = (__bthread_private *) scheduler->current_item;
    currentThread->state = __BTHREAD_ZOMBIE;
    currentThread->retval = retval;
}

#pragma clang diagnostic pop