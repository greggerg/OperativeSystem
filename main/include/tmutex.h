//
// Created by greg on 27/12/18.
//

#ifndef OPERATIVESYSTEM_TMUTEX_H
#define OPERATIVESYSTEM_TMUTEX_H

#include "tqueue.h"

typedef struct {
    void *owner;
    TQueue waiting_list;
} bthread_mutex_t;

// Defined only for "compatibility" with pthread
typedef struct {
} bthread_mutexattr_t;

// attr is ignored
int bthread_mutex_init(bthread_mutex_t *m, const bthread_mutexattr_t *attr);

int bthread_mutex_destroy(bthread_mutex_t *m);

int bthread_mutex_lock(bthread_mutex_t *m);

int bthread_mutex_trylock(bthread_mutex_t *m);

int bthread_mutex_unlock(bthread_mutex_t *m);

#endif //OPERATIVESYSTEM_TMUTEX_H
