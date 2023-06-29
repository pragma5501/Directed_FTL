
#include "queue.h"

_queue* q_init () {
        _queue* q = (_queue*)malloc(sizeof(_queue));
        q->head = NULL;
        q->tail = NULL;
        q->next = NULL;
        
        q->size = 0;

        return q;
}

_queue* q_push (_queue* , int data) {
        if (q->size >= QUEUE_SIZE) {
                printf("Queue is full\n");
                return q;
        }

        q->LBA[q->tail] = data;
        
        q->tail = (q->tail + 1) % (QUEUE_SIZE);
        q->size++;
        return q;
}       

int q_pop (_queue* q) {
        if (q->size == 0) {
                printf("Queue is empty\n");
                return -1;
        }
        int data = q->LBA[q->head];
        
        q->head = (q->head + 1) % (QUEUE_SIZE);


        q->size--;
        return data;
}

_queue* q_destroy (_queue* q) {
        free(q);
} 