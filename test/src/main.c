#include <stdio.h>
#include <stdlib.h>
#include "../include/bthread_test.h"
#include "../include/tqueue_test.h"

int main(int agrc, char *argv[]) {
    tqueue_test();
    bthread_test();
    return 0;
}
