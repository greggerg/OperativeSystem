
#include "../include/bthread.h"
#include "../include/bthread_private.h"
#include "../include/tqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
//disable notify for save_context
#pragma clang diagnostic push
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#define STACK_SIZE 8388608

static void printQueue(TQueue pNode);

__bthread_scheduler_private *bthread_get_scheduler() {
    static __bthread_scheduler_private *scheduler = NULL;
    if (scheduler == NULL) {
        printf("Creating the scheduler\n");
        scheduler = malloc(sizeof(__bthread_scheduler_private));
        scheduler->queue = NULL;
        scheduler->current_item = NULL;
        scheduler->scheduling_routine = roundRobin;
    }
    return scheduler;
};

static int bthread_check_if_zombie(bthread_t bthread, void **retval) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *thread = tqueue_get_data(scheduler->current_item);
    //Checks whether the thread referenced by the parameter bthread has reached a zombie state.
    if (thread->state == __BTHREAD_ZOMBIE) {
        trace("Zombie thread with tid:  %ld\n", thread->tid);
        if (retval != NULL) {
            //if retval is not NULL the exit status of the target thread (i.e. the value that was supplied to bthread_exit) is copied into the location pointed to by *retval;
            thread->retval = *retval;
            //the thread's stack is freed
            free(thread->stack);
            //the thread's private data structure is removed from the queue (Note: depending on your implementation, you might need to pay attention to the special case where
            // the scheduler's queue pointer itself changes!)
            TQueue q = scheduler->current_item;
            q = scheduler->current_item;
            tqueue_pop(&q);
        }
        return 1;
    } else {
        //If it's not the case the function returns 0.
        return 0;
    }
}

static void printQueue(TQueue pNode) {
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *tp = NULL;
    printf("\nCurrent list\n");
    for (int i = 0; i < tqueue_size(pNode); ++i) {
        tp = tqueue_get_data(scheduler->current_item);
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        printf("Thread %lu\n", tp->tid);
    }
}

/*
Creates a new thread structure and puts it at the end of the queue. The thread identifier (stored in the buffer pointed by bthread) corresponds to the position in the queue.
 The thread is not started when calling this function. Attributes passed through the attr argument are ignored
(thus it is possible to pass a NULL pointer). The stack pointer for new created threads is NULL.*/
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg,
                   unsigned int priority) {
    __bthread_private *thread = malloc(sizeof(__bthread_private));
    thread->body = start_routine;
    thread->arg = arg;
    thread->state = __BTHREAD_READY;
    thread->priority = priority;
    //TODO rimuovere
    if (attr != NULL) {
        thread->attr = *attr;
    }
    thread->stack = NULL;
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    thread->tid = tqueue_enqueue(&scheduler->queue, thread);
    *bthread = thread->tid;
    scheduler->current_item = scheduler->queue;
    trace("Created thread with tid:  %ld\n", thread->tid);
    return (int) thread->tid;
}

//TODO
void bthread_cleanup() {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    free(scheduler);
}

static TQueue bthread_get_queue_at(bthread_t bthread) {
    volatile __bthread_scheduler_private *sp = bthread_get_scheduler();
    return tqueue_at_offset(sp->queue, bthread);
}

/*Saves the thread context and restores (long-jumps to) the scheduler context. Saving using sigsetjmp,
 * which is similar to setjmp but can also save the signal mask if the provided additional parameter
 * is not zero (to restore both the context and the signal mask the corresponding call is siglongjmp).
 * Saving and restoring the signal mask is required for implementing preemption.*/
void bthread_yield() {
    bthread_block_timer_signal();//lock atomico
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *currentThread = (__bthread_private *) tqueue_get_data(scheduler->current_item);
    if (!save_context(currentThread->context)) {
        restore_context(scheduler->context);
    }
    bthread_unblock_timer_signal();//unlock
}


int bthread_join(bthread_t bthread, void **retval) {
    bthread_block_timer_signal();//lock atomico
    bthread_setup_timer();//setto il timer per la preemption
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval)) return 0;
    volatile __bthread_private *tp;
    do {
        //move to the next one
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = tqueue_get_data(scheduler->current_item);
        //see if someone needs to be waken up
        if (tp->state == __BTHREAD_SLEEPING) {
            if (tp->wake_up_time <= get_current_time_millis()) {
                printf("Thread %lu \n", tp->tid);
                tp->state = __BTHREAD_READY;
            }
        }
    } while (tp->state != __BTHREAD_READY);
    trace("Thread starts tid:  %ld\n", tp->tid);
    // Restore context or setup stack and perform first call
    if (tp->stack) {
        restore_context(tp->context);
    } else {
        printf("Setting the stack for the first time\n");
        tp->stack = (char *) malloc(sizeof(char) * STACK_SIZE);
#if __x86_64__
        asm __volatile__("movq %0, %%rsp"::
        "r"((intptr_t) (tp->stack + STACK_SIZE - 1)));
#else
        asm __volatile__("movl %0, %%esp" ::
           "r"((intptr_t) (tp->stack + STACK_SIZE - 1)));
#endif
        printf("A thread starts! \n");
        bthread_exit(tp->body(tp->arg));
    }
}

/*Terminates the calling thread and returns a value via retval that will be available to another
thread in the same process that calls bthread_join, then yields to the scheduler. Between
bthread_exit and the corresponding bthread_join the thread stays in the __BTHREAD_ZOMBIE state.*/
void bthread_exit(void *retval) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile __bthread_private *currentThread = (__bthread_private *) tqueue_get_data(scheduler->current_item);
    if (currentThread == NULL) {
        printf("Invalid thread\n");
//TODO remove if
    } else {
        printf("\n----------------------------------------------------------------\nExiting thread %lu\n",
               currentThread->tid);
        currentThread->state = __BTHREAD_ZOMBIE;
        currentThread->retval = retval;
        trace("Thread with tid:  %ld exiting\n", thread->tid);
        //  bthread_yield();
        bthread_printf("Yield\n");
    }
}

double get_current_time_millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (int) ((tv.tv_usec) / 1000);
}

void bthread_sleep(double ms) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *thread = tqueue_get_data(scheduler->current_item);
    thread->state = __BTHREAD_SLEEPING;
    trace("Thread with tid:  %ld sleeping\n", thread->tid);
    thread->wake_up_time = ms + get_current_time_millis();
    bthread_yield();
}

int bthread_cancel(bthread_t bthread) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile TQueue queue = tqueue_at_offset(scheduler->queue, bthread);
    //thread da terminare
    volatile __bthread_private *thread = tqueue_get_data(queue);
    thread->cancel_req = 1;
}

//Threads can also request cancellation of another thread: cancellation happens as soon as the thread receiving the request calls testcancel.
//Cancellation (through bthread_exit) happens when the recipient thread executes bthread_testcancel . The return value of a cancelled thread is -1
void bthread_testcancel(void) {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *thread = tqueue_get_data(scheduler->current_item);
    if (thread->cancel_req == 1) {
        bthread_exit((void *) -1);
    }
}

/*Preemption can be implemented by means of a timer signal that periodically interrupts the executing thread and returns control to the scheduler. To set such a timer we employ setitimer:*/
static void bthread_setup_timer() {
    static bool initialized = false;
    if (!initialized) {
        signal(SIGVTALRM, (void (*)()) bthread_yield);
        struct itimerval time;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = QUANTUM_USEC;
        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = QUANTUM_USEC;
        initialized = true;
        setitimer(ITIMER_VIRTUAL, &time, NULL);
    }
}

//lock atomico bloccando il segnale
void bthread_block_timer_signal() {
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &signal, NULL);//blocca
}

// Unlocka il segnale
void bthread_unblock_timer_signal() {
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &signal, NULL);//sblocca
}

//Scheduling procedures
void roundRobin() {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
}

void priority() {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile __bthread_private *thread;
    TQueue queue = NULL;
    unsigned long size = tqueue_size(scheduler->queue);
    volatile unsigned int priorityLevel = 999;
    unsigned long i;
    for (i = 0; i < size; i++) {
        TQueue tempQueue = tqueue_at_offset(scheduler->queue, i);
        thread = tqueue_get_data(tempQueue);
        if (thread->state != __BTHREAD_BLOCKED) {
            if (thread->priority < priorityLevel) {
                priorityLevel = thread->priority;
                queue = tempQueue;
            }
        }
    }
    scheduler->current_item = queue;
}

void randomChoice() {
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile __bthread_private *thread;
    TQueue queue;
    do {
        unsigned long randID = rand() % tqueue_size(scheduler->queue);
        queue = tqueue_at_offset(scheduler->queue, randID);
        thread = tqueue_get_data(queue);
    } while (!(thread->state == __BTHREAD_READY));
    scheduler->current_item = queue;
}

//sostituisce la macro
void bthread_printf(const char *format, ...) // requires stdlib.h and stdarg.h
{
    bthread_block_timer_signal();
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    bthread_unblock_timer_signal();
}

void setSchedulingRoutine(bthread_scheduling_routine routine) {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->scheduling_routine = routine;
}
