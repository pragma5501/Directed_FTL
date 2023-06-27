#include <stdio.h>

#define PAGE_SIZE 4 * 1024 // Byte
#define BLOCK_SIZE 512 * (PAGE_SIZE) // 512 page // Byte
#define SSD_SIZE 32 * 1024 * 1024 * 1024 * 1024

#define PAGE_NUM 512
#define BLOCK_NUM 2 * 1024 * 1024 


typedef struct block {
        int page_bitmap[PAGE_NUM];
} block_t;


typedef struct ssd {
        block_t* block[BLOCK_NUM];

        // traffic for WAF
        long long traff_client;
        long long traff_ftl;
} ssd_t;

double get_WAF (ssd_t* my_ssd);