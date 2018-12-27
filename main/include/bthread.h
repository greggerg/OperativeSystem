#include <setjmp.h>
#include "tqueue.h"
#include "bthread_private.h"

#ifndef OPERATIVESYSTEM_BTHREAD_H
#define OPERATIVESYSTEM_BTHREAD_H
/*#define bthread_printf(...) \
printf(__VA_ARGS__); \
bthread_yield();
*/
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg,unsigned int priority);

int bthread_join(bthread_t bthread, void **retval);

void bthread_yield();

void bthread_sleep(double ms);

int bthread_cancel(bthread_t bthread);

void bthread_testcancel(void);

void bthread_exit(void *retval);

void bthread_printf(const char *format, ...);

void setSchedulingRoutine(bthread_scheduling_routine routine);
#endif