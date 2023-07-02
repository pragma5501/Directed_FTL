#include "FTL.h"


int64_t mapping_table[M_TABLE_SIZE];


ssd_t* ssd_t_init () {
        ssd_t* my_ssd = (ssd_t*)malloc(sizeof(ssd_t));
        
        my_ssd->block = block_t_init(my_ssd->block);
        
        my_ssd->traff_client = 0;
        my_ssd->traff_ftl    = 0;

        my_ssd->flag_GC      = GC_F;
        return my_ssd; 
}

block_t** block_t_init (block_t** my_block) {  
        my_block = (block_t**)malloc(sizeof(block_t*) * BLOCK_NUM);

        int64_t i;
        for (i = 0; i < BLOCK_NUM; i++) {

                my_block[i] = (block_t*)malloc(sizeof(block_t));
                my_block[i]->invalid_page_num = 0;

                page_init(my_block[i]);
        }
        return my_block;
}

void page_init (block_t* my_block) {
        my_block->LBA = (int64_t*)malloc(sizeof(int64_t) * PAGE_NUM);
        my_block->page_bitmap = (int*)malloc(sizeof(int) * PAGE_NUM);

        int64_t i;
        for (i = 0; i < PAGE_NUM; i++) {
                my_block->page_bitmap[i] = ERASED;
                
        }

}


int* page_erase (int* page) {
        int64_t i;
        for(i = 0; i < PAGE_NUM; i++) {
                page[i] = ERASED;
        }
        return page;
}


ssd_t* ssd_t_write (ssd_t* my_ssd, int64_t PPN, int page_bit, int64_t LBA) {
        // get the position of page from PBN
        int64_t block_n = (int64_t)(PPN / (PAGE_NUM));
        int64_t page_n = PPN % (PAGE_NUM);

        // just write
        my_ssd->block[block_n]->page_bitmap[page_n] = page_bit;
        my_ssd->block[block_n]->LBA[page_n] = LBA;

        if (my_ssd->flag_GC == GC_T) {
                mapping_table[LBA] = PPN;
        }

        // if invalid 
        if (page_bit == INVALID) {
                my_ssd->block[block_n]->invalid_page_num += 1;
                //printf("block %ld invalid : %d\n",  block_n, my_ssd->block[block_n]->invalid_page_num);
                return my_ssd;
        }

        
        // traffic 
        my_ssd->traff_ftl += 1;

        return my_ssd;
}



void destroy_ssd (ssd_t* my_ssd) {
        int64_t i;
        for (i = 0; i < BLOCK_NUM; i++) {
                free(my_ssd->block[i]->page_bitmap);
                free(my_ssd->block[i]);
        }
        free(my_ssd->block);
        free(my_ssd);
}



// free block queue
_queue* free_q_init (_queue* q) {
        int64_t i;
        for (i = 0; i < (int64_t)QUEUE_SIZE; i++) {
                q_push(q, i);
        }
        printf("queue size : %ld\n", q->size);
        return q;
}

int64_t free_q_pop (ssd_t* my_ssd, _queue* free_q) {
        int64_t PPN = q_pop(free_q);

        if (free_q->size < (THRESHOLD_FREE_Q)) {
                GC(my_ssd, free_q);
        }
        return PPN;
}

void init_mapping_table () {
        int64_t i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                mapping_table[i] = -1;
        }
}



ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int64_t LBA) {

        // Physhical Page Number
        int64_t PPN;

        // if modify
        if (mapping_table[LBA] != -1) {
                PPN = mapping_table[LBA];
                my_ssd = ssd_t_write(my_ssd, PPN, INVALID, LBA);
        }

        PPN = free_q_pop(my_ssd, free_q);
        mapping_table[LBA] = PPN;

        // printf("LBA -> PPN : %ld -> %ld\n", LBA, PPN);
        my_ssd = ssd_t_write(my_ssd, PPN, VALID, LBA);

        my_ssd->traff_client += 1;
        return my_ssd;
}

int GC (ssd_t* my_ssd, _queue* free_q) {
        //printf("GC on\n");
        if (my_ssd->flag_GC == GC_T) {
                return -1;
        }
        my_ssd->flag_GC = GC_T;

        int64_t block_n_victim = get_victim(my_ssd);

        //printf("invallid_page_num : %d\n", my_ssd->block[block_n_victim]->invalid_page_num);
        block_t* block_victim = my_ssd->block[block_n_victim];

        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                int page_bit = block_victim->page_bitmap[i];

                q_push(free_q, block_n_victim * PAGE_NUM + i);


                if (page_bit == INVALID) {
                        continue;
                }

                // a page_bit is valid
                int64_t PPN = free_q_pop(my_ssd, free_q);
                int64_t LBA = block_victim->LBA[i];
                ssd_t_write(my_ssd, PPN, VALID, LBA);
        }

        page_erase(my_ssd->block[block_n_victim]->page_bitmap);
        my_ssd->block[block_n_victim]->invalid_page_num = 0;
        my_ssd->flag_GC = GC_F;
}

// select a block that has most invalid page
int64_t get_victim (ssd_t* my_ssd) {
        int max = -1;
        int64_t max_i = -1;
        int64_t i;
        for (i = 0; i < BLOCK_NUM; i++ ) {
                int tmp = my_ssd->block[i]->invalid_page_num;
                // printf("tmp: %d\n", tmp);
                max_i = (tmp > max) ? i   : max_i;
                max =  (tmp > max)  ? tmp : max;
                
        }

        return max_i;
}