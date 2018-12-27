//
// Created by greg on 27/12/18.
//

#ifndef OPERATIVESYSTEM_TBARRIER_H
#define OPERATIVESYSTEM_TBARRIER_H
#include "../include/tqueue.h"

typedef struct {
    TQueue waiting_list;
    unsigned count;
    unsigned barrier_size;
} bthread_barrier_t;

// Defined only for "compatibility" with pthread
typedef struct {
} bthread_barrierattr_t;

// attr is ignored
int bthread_barrier_init(bthread_barrier_t *b, const bthread_barrierattr_t *attr, unsigned count);

int bthread_barrier_destroy(bthread_barrier_t *b);

int bthread_barrier_wait(bthread_barrier_t *b);

#endif //OPERATIVESYSTEM_TBARRIER_H
