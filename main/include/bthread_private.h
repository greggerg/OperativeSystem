#ifndef OPERATIVESYSTEM_BTHREAD_PRIVATE_H
#define OPERATIVESYSTEM_BTHREAD_PRIVATE_H

#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

typedef unsigned long int bthread_t;

typedef struct {
} bthread_attr_t;

typedef enum {
    __BTHREAD_READY = 0, __BTHREAD_BLOCKED, __BTHREAD_SLEEPING, __BTHREAD_ZOMBIE
} bthread_state;

typedef void *(*bthread_routine)(void *);

//bthread structure
typedef struct {
    bthread_t tid;
    bthread_routine body;
    //body argument
    void *arg;
    bthread_state state;
    bthread_attr_t attr;
    char *stack;
    //stack context
    jmp_buf context;
    void *retval;
} __bthread_private;

//scheduler
typedef struct {
    TQueue queue;
    TQueue current_item;
    jmp_buf context;
    bthread_t current_tid;
} __bthread_scheduler_private;


__bthread_scheduler_private *bthread_get_scheduler();

void bthread_cleanup();

static TQueue bthread_get_queue_at(bthread_t bthread);

static int bthread_check_if_zombie(bthread_t bthread, void **retval);

#endif