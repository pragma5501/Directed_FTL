#include <stdio.h>
#include "ssd.h"
#include "util/queue.h"

#define L_BLOCK_SIZE 4 * 1024

#define M_TABLE_SIZE 8589934592 // 8 * 1024 * 1024 * 1024

enum IO {
        ERASED = 1, // Erased status
        VALID = 0, // Valid  status
};

long long mapping_table[M_TABLE_SIZE];

ssd_t* ssd_t_init ();
block_t* block_t_init (ssd_t* my_ssd);
int* page_init (block_t* my_block);

block_t* block_t_read (ssd_t* my_ssd, int block_n);
block_t* block_t_erase (block_t* ssd_block);
block_t* block_t_write (block_t* ssd_block, block_t* tmp_block);

ssd_t* ssd_t_write (ssd_t* my_ssd, int PBN);

void destroy_ssd (ssd_t* my_ssd);

_queue* free_q_init (_queue* q);

void* init_mapping_table();

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA);