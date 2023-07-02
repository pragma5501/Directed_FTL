#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define QUEUE_SIZE 8589934592

typedef struct _q {
        int64_t head;
        int64_t tail;
        int64_t size;
        int64_t LBA[QUEUE_SIZE];
} _queue;

_queue* q_init();
_queue* q_push(_queue* q, int64_t data);
int64_t q_pop(_queue* q);
_queue* q_destroy(_queue* q);