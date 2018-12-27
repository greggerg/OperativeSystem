//
// Created by greg on 27/12/18.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/tbarrier.h"
#include "../include/bthread_private.h"
#include "../include/bthread.h"

int bthread_barrier_init(bthread_barrier_t *b, const bthread_barrierattr_t *attr, unsigned count) {
    assert(b!=NULL);
    b->waiting_list = NULL;

    b->count=0;
    b->barrier_size=count;
    return 0;
}
//TODO: Chiedere a Brocco perché viene generato un SIGABRT signal durante il free()....
int bthread_barrier_destroy(bthread_barrier_t *b) {
    assert(tqueue_size(b->waiting_list) == 0);
    free(b->waiting_list);
    return 0;
}

int bthread_barrier_wait(bthread_barrier_t *b) {
    bthread_block_timer_signal();
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *thread = (__bthread_private *) tqueue_get_data(scheduler->current_item);

    if (b->count > b->barrier_size) {
        b->count++;
        thread->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&b->waiting_list, thread);
        bthread_yield();
        printf("aggiunto elemento nella coda\n");
    } else {
        for (int i = 0; i < tqueue_size(b->waiting_list); i++) {
            printf("faccio partire tutti i thread\n");
            __bthread_private *tempThread = tqueue_pop(&b->waiting_list);
            tempThread->state = __BTHREAD_READY;
        }
        b->count = 0;
    }
    bthread_unblock_timer_signal();
    return 0;
}

