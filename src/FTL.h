#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "ssd.h"
#include "util/queue.h"

#define L_BLOCK_SIZE 4 * 1024

#define M_TABLE_SIZE 8 * 1024 * 1024 * 100



#define THRESHOLD_FREE_Q 8354004992

enum IO {
        ERASED = 1, // Erased status
        VALID = 0, // Valid  status
        INVALID = 2,

        GC_T = 0,
        GC_F = 1,
};


ssd_t* ssd_t_init ();
block_t** block_t_init (block_t** my_block);
void page_init (block_t* my_block);

int64_t* page_read (ssd_t* my_ssd, int64_t block_n, int64_t* page_tmp);
int* page_erase (int* page);
int* page_write (int* ssd_page, int* page_tmp);

ssd_t* ssd_t_write (ssd_t* my_ssd, int64_t PPN, int page_bit, int64_t LBA);

void destroy_ssd (ssd_t* my_ssd);

_queue* free_q_init (_queue* q);
int64_t free_q_pop (ssd_t* my_ssd,_queue* free_q);
void init_mapping_table ();

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int64_t LBA);

// Garbage Colloection
int GC (ssd_t* my_ssd, _queue* free_q);
int64_t get_victim (ssd_t* my_ssd);