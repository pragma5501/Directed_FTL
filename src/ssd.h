#include <stdio.h>
#include <stdint.h>

#define PAGE_SIZE 4 * 1024 // Byte
#define BLOCK_SIZE 512 * (PAGE_SIZE) // 512 page // Byte
#define SSD_SIZE 32 * 1024 * 1024 * 1024

#define PAGE_NUM 512
#define BLOCK_NUM 4 * 16 * 1024


typedef struct block {
        int offset;
        int* LBA;

        int *page_bitmap;  // status
        int invalid_page_num;
} block_t;


typedef struct ssd {
        block_t **block;
        int       idx_block;
        
        block_t  *block_op;
        int       idx_block_op;
        // traffic for WAF
        int64_t traff_client;
        int64_t traff_ftl;

        int flag_GC;
} ssd_t;



double get_WAF (ssd_t* my_ssd);
// last : LBA -> PPN : 10327322 -> 1048576