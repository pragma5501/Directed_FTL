#include "FTL.h"
#include "err.h"

int mapping_table[M_TABLE_SIZE];



ssd_t* ssd_t_init () {
        ssd_t* my_ssd = (ssd_t*)malloc(sizeof(ssd_t));

        // malloc error handling
        my_assert(handle_allc_err(my_ssd));

        block_t_init(my_ssd);

        my_ssd->traff_client = 0;
        my_ssd->traff_ftl    = 0;

        my_ssd->flag_GC      = GC_F;
        return my_ssd;
}

ssd_t* block_t_init (ssd_t* my_ssd) {
        block_t** my_block = (block_t**)malloc(sizeof(block_t*) * BLOCK_NUM);
        my_assert(handle_allc_err(my_block));

        int i;
        for (i = 0; i < BLOCK_NUM; i++) {

                my_block[i] = (block_t*)malloc(sizeof(block_t));
                my_assert(handle_allc_err(my_block[i])); 

                // init page info
                my_block[i]->offset = i * (PAGE_NUM);
                my_block[i]->invalid_page_num = 0;

                page_init(my_block[i]);
        }
        
        block_t* block_op = (block_t*)malloc(sizeof(block_t));
        my_assert(handle_allc_err(block_op));


        my_ssd->block         = my_block;
        my_ssd->block_op      = block_op;
        my_ssd->idx_block_op  = PAGE_NUM;
        return my_ssd;
}


void page_init (block_t* my_block) {
        my_block->LBA = (int*)malloc(sizeof(int) * PAGE_NUM);
        my_block->page_bitmap = (int*)malloc(sizeof(int) * PAGE_NUM);

        // malloc error handling
        my_assert(handle_allc_err(my_block->LBA));
        my_assert(handle_allc_err(my_block->page_bitmap)); 

        int i;
        for (i = 0; i < PAGE_NUM; i++) {
                my_block->page_bitmap[i] = ERASED;
        }

}


int* page_erase (int* page) {
        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                page[i] = ERASED;
        }
        return page;
}


ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit, int LBA) {
        // get the position of page from PBN
        int block_n = (int)(PPN / (PAGE_NUM));
        int page_n = PPN % (PAGE_NUM);
        //printf("block offset : %d\n", block_n);
        // just write
        my_ssd->block[block_n]->page_bitmap[page_n] = page_bit;
        my_ssd->block[block_n]->LBA[page_n] = LBA;

        // if invalid 
        if (page_bit == INVALID) {
                //printf("invalid block offset : %d\n", block_n);
                my_ssd->block[block_n]->invalid_page_num += 1;
                return my_ssd;
        }

        mapping_table[LBA] = PPN;

        // traffic 
        my_ssd->traff_ftl += 1;

        return my_ssd;
}



void destroy_ssd (ssd_t* my_ssd) {
        int i;
        for (i = 0; i < BLOCK_NUM; i++) {
                free(my_ssd->block[i]->page_bitmap);
                free(my_ssd->block[i]->LBA);
                free(my_ssd->block[i]);
        }

        free(my_ssd->block);
        free(my_ssd->block_op);
        free(my_ssd);
}

int get_PPN (ssd_t* my_ssd) {
        return my_ssd->idx_block_op + my_ssd->block_op->offset;
        
}

// free block queue
_queue* free_q_init (ssd_t* my_ssd, _queue* q) {
        int i;

        for (i = 0; i < BLOCK_NUM; i++) {
                q_push(q, my_ssd->block[i]);
        }

        printf("queue size : %d\n", q->size);
        return q;
}

int free_q_pop (ssd_t* my_ssd, _queue* free_q) {
        
        if (my_ssd->idx_block_op >= PAGE_NUM) {
                my_ssd->idx_block_op = 0;
                my_ssd->block_op = q_pop(free_q);
                
                GC(my_ssd, free_q);
        }

        int PPN = get_PPN(my_ssd);
        my_ssd->idx_block_op += 1;

        return PPN;
}



void init_mapping_table () {
        int i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                mapping_table[i] = -1;
        }
}



ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA) {
        int PPN;

        // if modify
        if (mapping_table[LBA] != -1) {
                PPN = mapping_table[LBA];
                my_ssd = ssd_t_write(my_ssd, PPN, INVALID, LBA);
        }

        PPN = free_q_pop(my_ssd, free_q);

        //printf("LBA -> PPN : %d -> %d\n", LBA, PPN);
        my_ssd = ssd_t_write(my_ssd, PPN, VALID, LBA);

        my_ssd->traff_client += 1;
        return my_ssd;
}

int GC (ssd_t* my_ssd, _queue* free_q) {
        if (my_ssd->flag_GC == GC_T) {
                return -1;
        }

        if (free_q->size > THRESHOLD_FREE_Q) {
                return -1;
        }
        //printf("GC on\n");
        //printf("free_q size : %d\n", free_q->size);
        my_ssd->flag_GC = GC_T;

        int block_n_victim = get_victim(my_ssd);
        block_t* block_victim = my_ssd->block[block_n_victim];

        int i;
        for(i = 0; i < PAGE_NUM; i++) {

                int page_bit = block_victim->page_bitmap[i];
                if (page_bit == INVALID) {
                        continue;
                }

                // a page_bit is valid
                int PPN = free_q_pop(my_ssd, free_q);
                int LBA = block_victim->LBA[i];
                ssd_t_write(my_ssd, PPN, VALID, LBA);
        }
        //printf("GC result\n");
        //printf("victim block : %d\n", block_n_victim);
        //printf("invalid page num : %d\n", my_ssd->block[block_n_victim]->invalid_page_num);

        // page_erase(my_ssd->block[block_n_victim]->page_bitmap);
        my_ssd->block[block_n_victim]->invalid_page_num = 0;

        q_push(free_q, my_ssd->block[block_n_victim]);


        my_ssd->flag_GC = GC_F;
}

// select a block that has most invalid page
int get_victim (ssd_t* my_ssd) {

        int max = -1;
        int max_i = -1;
        int i;

        for (i = 0; i < BLOCK_NUM; i++ ) {
                int tmp = my_ssd->block[i]->invalid_page_num;

                max_i = (tmp > max) ? i   : max_i;
                max =  (tmp > max)  ? tmp : max;
                
        }

        return max_i;
}