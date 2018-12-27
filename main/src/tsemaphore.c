//
// Created by greg on 27/12/18.
//

#include <stdlib.h>
#include "../include/tsemaphore.h"
#include "../include/bthread_private.h"

//Crea il semaforo con dimensione impostata a parametro
int bthread_sem_init(bthread_sem_t *s, int pshared, int value) {
    if (s == NULL) {
        s = (bthread_sem_t *) malloc(sizeof(bthread_sem_t));
    }
    s->value = value;
    s->waiting_list = NULL;
    return 0;
}
//Libera la memoria allocata dal semaforo
int bthread_sem_destroy(bthread_sem_t *s) {
    if(tqueue_size(s->waiting_list)==0){
        free(s->waiting_list);
    }
    return 0;
}
//Decrementa in modo atomico il valore del semaforo
int bthread_sem_wait(bthread_sem_t *s) {
    bthread_block_timer_signal();
    s->value--;
    bthread_unblock_timer_signal();
    return 0;
}
//Incrementa in modo atomico value del semaforo
int bthread_sem_post(bthread_sem_t *s) {
    bthread_block_timer_signal();
    s->value++;
    bthread_unblock_timer_signal();
    return 0;
}

