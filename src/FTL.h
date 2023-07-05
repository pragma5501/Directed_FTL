#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "queue.h"



#define M_TABLE_SIZE 4 * 8 * 1024 * 1024 

#define THRESHOLD_FREE_Q 5



enum IO {
        ERASED = 1, // Erased status
        VALID = 0, // Valid  status
        INVALID = 2,

        GC_T = 0,
        GC_F = 1,
};




ssd_t* ssd_t_init ();
ssd_t* block_t_init (ssd_t* my_ssd);
void page_init (block_t* my_block);

int* page_erase (int* page);

ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit, int LBA);

void destroy_ssd (ssd_t* my_ssd);

_queue* free_q_init (ssd_t* my_ssd, _queue* q);
int free_q_pop (ssd_t* my_ssd,_queue* free_q);
void init_mapping_table ();

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA);

// Garbage Colloection
int GC (ssd_t* my_ssd, _queue* free_q);
int get_victim (ssd_t* my_ssd);