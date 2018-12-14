#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../main/include/bthread.h"

void *routine(void *arg) {
    printf("Hello thread %s\n", (char *) arg);
    return arg;
}

void bthread_test() {
    bthread_t threads[10];
    void *arg = "Greg";
    void *retval = "42";

//create
    printf("Trying\n");
    for (unsigned long i = 0; i < 10; ++i) {
        threads[i] = i;
        bthread_create(&threads[i], NULL, routine, arg);
    }
    printf("Threads created!\n");
//join
    for (int j = 0; j < 10; ++j) {
        int result = bthread_join(threads[j], &retval);
        printf("Result of join %d\n", result);
    }
    printf("Threads joined!\n");
    printf("bthread test succeeded\n");
}
