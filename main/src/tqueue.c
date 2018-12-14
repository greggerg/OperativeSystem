#include <stdio.h>
#include <stdlib.h>
#include "../include/tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode *next;
    void *data;
} TQueueNode;

/* Adds a new element at the end of the list, returns its position */
unsigned long int tqueue_enqueue(TQueue *q, void *data) {
    TQueueNode* new_element = (TQueueNode*) malloc(sizeof(TQueueNode));
    new_element->data = data;
    unsigned long int index = 0;
    if (*q == NULL) {
        *q = new_element;
        new_element->next = *q;
    } else {
        TQueueNode* head = *q;
        index = 1;
        while(head->next != *q) {
            head = head->next;
            index++;
        }
        head->next = new_element;
        new_element->next = *q;
    }
    return index;
    }
/* Removes and returns the element at the beginning of the list, NULL if the
queue is empty */
void *tqueue_pop(TQueue *q) {
    unsigned long size = tqueue_size(*q);
    TQueueNode *toRemove=NULL;
    if (size == 0) {
        return NULL;
    } else {
        TQueueNode *current = *q;
        if (current->next == current) {
            //rimuovo ultimo nodo
            *q = NULL;
        } else {
            for (int i = 0; i < size - 1; i++) {
                current = current->next;
            }
            toRemove = *q;
            //attacco ultimo nodo al secondo
            current->next = toRemove->next;
        }
        return toRemove;
    }
};

/* Returns the number of elements in the list */
unsigned long int tqueue_size(TQueue q) {
    if (q == NULL) {
        return 0;
    }
    //current node is head
    TQueueNode *head = q->next;
    TQueueNode *current = q->next;

    unsigned long size = 1;
    while (current->next != head) {
        size++;
        current = current->next;
    }
    //printf("Size %d\n",size);
    return size;
};

/* Returns a 'view' on the list starting at (a positive) offset distance,
 * NULL if the queue is empty */
TQueue tqueue_at_offset(TQueue q, unsigned long int offset) {
    if (q == NULL)
        return NULL;
    TQueueNode *current = q;
    for (int i = 0; i < offset; i++) {
        current = current->next;
    }
    return current;
};

/* Returns the data on the first node of the given list */
void *tqueue_get_data(TQueue q) {
    if (q == NULL) {
        printf("Error list empty\n");
    }
    return q->data;
};