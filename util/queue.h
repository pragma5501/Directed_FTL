#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 512 * 128

typedef struct _q {
        int head;
        int tail;
        int size;
        int LBA[QUEUE_SIZE];
} _queue;

_queue* q_init();
_queue* q_push(_queue* q, int data);
int q_pop(_queue* q);
_queue* q_destroy(_queue* q);