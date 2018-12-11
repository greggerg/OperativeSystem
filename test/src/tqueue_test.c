#include <stdlib.h>
#include <stdio.h>
#include "../../main/include/tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode *next;
    void *data;
}
        TQueueNode;

void tqueue_test() {
    TQueue list = NULL;
//test size 0
    unsigned long size = tqueue_size(list);
    if (size != 0) {
        printf("Error size of empty list not correct\n");
    };
//add elements
    int value = 17;
    tqueue_enqueue(&list, (void *) &value);
    int value2 = 34;
    tqueue_enqueue(&list, (void *) &value2);
    int value3 = 54;
    tqueue_enqueue(&list, (void *) &value3);
//test size
    size = tqueue_size(list);
    if (size != 3) {
        printf("Error size not correct\n");
    };
//pop
    TQueueNode *element = tqueue_pop(&list);
    int data = *(int *) element->data;
    if (data != 17) {
        printf("Error first element is not 17\n");
    }
//test size
    size = tqueue_size(list);
    if (size != 2) {
        printf("Error popped size not correct\n");
    };
//get data
    data = *(int *) tqueue_get_data(list);
    if (data != 17) {
        printf("Error head element is not 17\n");
    }
//get view
    TQueueNode *second = tqueue_at_offset(list, 1);
//test view
    data = *(int *) second->data;
    if (data != 34) {
        printf("Error second element is not 34\n");
    }
    printf("TQueue test succeded\n");
}