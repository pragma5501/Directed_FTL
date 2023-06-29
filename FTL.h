#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "ssd.h"
#include "util/queue.h"

#define L_BLOCK_SIZE 4 * 1024

#define M_TABLE_SIZE 8 * 1024 * 1024// 8589934592 // 8 * 1024 * 1024 * 1024

#define ST_BITMAP_SIZE (BLOCK_NUM) * (PAGE_NUM)

#define THRESHOLD_FREE_Q 10

enum IO {
        ERASED = 1, // Erased status
        VALID = 0, // Valid  status
        INVALID = 2,
};


ssd_t* ssd_t_init ();
block_t** block_t_init (block_t** my_block);
void page_init (block_t* my_block);

int* page_read (ssd_t* my_ssd, int block_n, int* page_tmp);
int* page_erase (int* page);
int* page_write (int* ssd_page, int* page_tmp);

ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit);

void destroy_ssd (ssd_t* my_ssd);

_queue* free_q_init (_queue* q);
int free_q_pop (_queue* free_q);
void init_mapping_table ();

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA);

// Garbage Colloection
void GC (ssd_t* my_ssd, _queue* free_q);
int get_victim (ssd_t* my_ssd);