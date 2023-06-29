#include "FTL.h"

long long mapping_table[M_TABLE_SIZE];



ssd_t* ssd_t_init () {
        ssd_t* my_ssd = (ssd_t*)malloc(sizeof(ssd_t));
        
        my_ssd->block = block_t_init(my_ssd->block);
        
        my_ssd->traff_client = 0;
        my_ssd->traff_ftl    = 0;

        return my_ssd; 
}

block_t** block_t_init (block_t** my_block) {  
        my_block = (block_t**)malloc(sizeof(block_t*) * BLOCK_NUM);
        int i;
        for (i = 0; i < BLOCK_NUM; i++) {
                my_block[i] = (block_t*)malloc(sizeof(block_t));
                my_block[i]->invalid_page_num = 0;
                page_init(my_block[i]);
        }
        return my_block;
}

void page_init (block_t* my_block) {
        my_block->page_bitmap = (int*)malloc(sizeof(int) * PAGE_NUM);

        int i;
        for (i = 0; i < PAGE_NUM; i++) {
                my_block->page_bitmap[i] = ERASED;
                
        }

}

int* page_read (ssd_t* my_ssd, int block_n, int* page_tmp) {
        memcpy(page_tmp, my_ssd->block[block_n]->page_bitmap, sizeof(int) * (PAGE_NUM));
        return page_tmp;
}
int* page_erase (int* page) {
        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                page[i] = ERASED;
        }
        return page;
}
int* page_write (int* ssd_page, int* page_tmp) {
        page_erase(ssd_page);
        memcpy(ssd_page, page_tmp, sizeof(int) * PAGE_NUM);
        
        return ssd_page;
}

ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit) {
        // get the position of page from PBN
        int block_n = (int)(PPN / (PAGE_NUM));
        int page_n = PPN % (PAGE_NUM);

        // just write
        my_ssd->block[block_n]->page_bitmap[page_n] = page_bit;

        // if invalid 
        if (page_bit == INVALID) {
                my_ssd->block[block_n]->invalid_page_num += 1;
        }
        // traffic 
        my_ssd->traff_ftl    += 1;

        return my_ssd;
}



void destroy_ssd (ssd_t* my_ssd) {
        int i;
        for (i = 0; i < BLOCK_NUM; i++) {
                free(my_ssd->block[i]->page_bitmap);
                free(my_ssd->block[i]);
        }
        free(my_ssd->block);
        free(my_ssd);
}



// free block queue
_queue* free_q_init (_queue* q) {
        int i;
        for (i = 0; i < (int)QUEUE_SIZE; i++) {
                q_push(q, i);
        }
        printf("queue size : %d\n", q->size);
        return q;
}

int free_q_pop (ssd_t* my_ssd, _queue* free_q) {
        int PPN = q_pop(free_q);

        if (free_q->size < (THRESHOLD_FREE_Q) * (PAGE_NUM)) {
                GC(my_ssd, free_q);
        }
        return PPN;
}

void init_mapping_table () {
        int i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                mapping_table[i] = -1;
        }
}



ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA) {

        // Physhical Page Number
        int PPN;
        // if modify
        if (mapping_table[LBA] != -1) {
                PPN = mapping_table[LBA];
                ssd_t_write(my_ssd, PPN, INVALID);
                printf("WAF : %2f\n\n", get_WAF(my_ssd));
        }

        PPN = free_q_pop(my_ssd, free_q);
        mapping_table[LBA] = PPN;

        printf("LBA -> PPN : %d -> %d\n", LBA, PPN);
        ssd_t_write(my_ssd, PPN, VALID);
        my_ssd->traff_client += 1;

        
        return my_ssd;
}

void GC (ssd_t* my_ssd, _queue* free_q) {
        printf("GC on\n");
        int block_n_victim = get_victim(my_ssd);
        printf("victim block : %d\n", block_n_victim);
        printf("invallid_page_num : %d\n", my_ssd->block[block_n_victim]->invalid_page_num);
        block_t* block_victim = my_ssd->block[block_n_victim];

        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                int page_bit = block_victim->page_bitmap[i];
                q_push(free_q, block_n_victim * PAGE_NUM + i);
                if (page_bit == INVALID) {
                        printf("find_invalid");
                        continue;
                }
                // a page_bit is valid
                int PPN = q_pop(free_q);
                ssd_t_write(my_ssd, PPN, VALID);

                
        }

        page_erase(my_ssd->block[block_n_victim]->page_bitmap);
}

// select a block that has most invalid page
int get_victim (ssd_t* my_ssd) {
        int max = INT_MIN;

        int i;
        for (i = 0; i < BLOCK_NUM; i++ ) {
                int tmp = my_ssd->block[i]->invalid_page_num;
                max = (tmp > max) ? tmp : max;
        }
        return max;
}