#include <stdio.h>
#include <stdlib.h>
#include "../include/tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode *next;
    void *data;
}TQueueNode;

/* Adds a new element at the end of the list, returns its position */
unsigned long int tqueue_enqueue(TQueue *q, void *data) {
    TQueueNode *elem = (TQueueNode *) malloc(sizeof(TQueueNode));

    if (*q == NULL) {
        elem->data = data;
        elem->next = elem;
        *q = elem;
        return 0;
    } else {
        //lista non vuota scorro fino alla fine e aggiungo il nodo
        TQueueNode *current = (*q)->next;
        unsigned long int size = tqueue_size(*q);
        for (int i = 0; i < size - 1; i++) {
            current = current->next;
        }
        //creo nodo dinamicamente
        //assegno i dati
        elem->data = data;
        //lo attacco alla testa
        elem->next = (*q)->next;
        //attacco ultimo nodo al nodo appena creato
        current->next = elem;
        return size;
    }
};

/* Removes and returns the element at the beginning of the list, NULL if the
queue is empty */
void *tqueue_pop(TQueue *q) {
    unsigned long size = tqueue_size(*q);
    if (size == 0) {
        return NULL;
    } else {
        TQueueNode *current = *q;
        void *retval = current->data;
        for (int i = 0; i < size-1; i++) {
            current = current->next;
        }
        current->next = *q;
       free(current);
        return retval;
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
    TQueueNode *current = q->next;
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