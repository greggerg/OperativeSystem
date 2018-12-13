#include <stdio.h>
#include <stdlib.h>
#include "../../main/include/bthread.h"

void *routine(void *arg) {
    printf("Hello thread %s\n", (char *) arg);
    return arg;
}

void bthread_test() {
    bthread_t bthread;
    bthread_t bthread2;
    void *arg = "Greg";
    void *arg2 = "Mike";
//create
    printf("Trying\n");
    int result = bthread_create(&bthread, NULL, routine, arg);
    int result2 = bthread_create(&bthread2, NULL, routine, arg2);
//start

//join
    void *retval = "42s";
    printf("Trying to join\n");
    bthread_join(17, &retval);
    bthread_join(2, &retval);


//yield
//exit
    printf("bthread test succeded\n");
}
