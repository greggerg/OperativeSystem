//
// Created by greg on 27/12/18.
//

#include <stdlib.h>
#include <assert.h>
#include "../include/tcondition.h"
#include "../include/bthread_private.h"
#include "../include/bthread.h"

// inizializzazione
int bthread_cond_init(bthread_cond_t *c, const bthread_condattr_t *attr) {
    //attr è ignorato
    if(c != NULL){
        return 1;
    } else {
        c = (bthread_cond_t*) malloc(sizeof(bthread_cond_t));
        c->waiting_list = NULL;
        return 0;
    }}
// Libera l'allocazione di memoria della condizione di bthread_cond_t
int bthread_cond_destroy(bthread_cond_t *c) {
    assert(tqueue_size(c->waiting_list) == 0);
    free(c->waiting_list);
    return 0;
}
// Funzione usata per bloccare con una condizione variabile un processo e aspettare una determinata condizione di sblocco.
int bthread_cond_wait(bthread_cond_t *c, bthread_mutex_t *mutex) {
    bthread_block_timer_signal();
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* thread = tqueue_get_data(scheduler->current_item);
    if(mutex->owner == thread){
        __bthread_private* tempThread = tqueue_pop(&mutex->waiting_list);

        if(tempThread == NULL){
            mutex->owner = NULL;
        } else {
            tempThread->state = __BTHREAD_READY;
            mutex->owner = tempThread;
        }
    }
    thread->state = __BTHREAD_BLOCKED;
    tqueue_enqueue(&c->waiting_list, thread);
    while(thread->state == __BTHREAD_BLOCKED){
        bthread_yield();
    }
    bthread_mutex_lock(mutex);
    bthread_unblock_timer_signal();
}

// Funzione per notify ad un thread
int bthread_cond_signal(bthread_cond_t *c) {
    bthread_block_timer_signal();
    if(tqueue_size(c->waiting_list) > 0){
        __bthread_private* thread = tqueue_pop(&c->waiting_list);
        thread->state = __BTHREAD_READY;
    }
    bthread_unblock_timer_signal();
}
// Funzione per notify-all su tutti i threads
int bthread_cond_broadcast(bthread_cond_t *c) {
    bthread_block_timer_signal();
    for(int i = 0; i < tqueue_size(c->waiting_list); i++){
        __bthread_private* thread = tqueue_pop(&c->waiting_list);
        thread->state = __BTHREAD_READY;
    }
    bthread_unblock_timer_signal();
}

