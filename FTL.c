#include "FTL.h"

long long mapping_table[M_TABLE_SIZE];

ssd_t* ssd_t_init () {
        ssd_t* my_ssd = (ssd_t*)malloc(sizeof(ssd_t));
        block_t_init(my_ssd);
        
        my_ssd->traff_client = 0;
        my_ssd->traff_ftl    = 0;

        return my_ssd; 
}

block_t* block_t_init (ssd_t* my_ssd) {  
        int i;
        for (i = 0; i < BLOCK_NUM; i++) {
                page_init(&my_ssd->block[i]);
        }

}

int* page_init (block_t* my_block) {
        int i;
        for (i = 0; i < PAGE_NUM; i++) {
                my_block->page_bitmap[i] = ERASED;
        }

}

int* page_read (ssd_t* my_ssd, int block_n, int* page_tmp) {
        memcpy(page_tmp, my_ssd->block[block_n].page_bitmap, sizeof(int) * (PAGE_NUM));
        return page_tmp;
}
int* page_erase (int* page) {
        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                page[i] = ERASED;
        }
        return page;
}
int* page_write(int* ssd_page, int* page_tmp) {
        page_erase(ssd_page);
        memcpy(ssd_page, page_tmp, sizeof(int) * PAGE_NUM);

        return ssd_page;
}

block_t* block_t_read (ssd_t* my_ssd, int block_n) {

        return &my_ssd->block[block_n];
}

block_t* block_t_erase (block_t* ssd_block) {
        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                ssd_block->page_bitmap[i] = ERASED;
        }
        return ssd_block;
}

block_t* block_t_write (block_t* ssd_block, block_t* tmp_block) {
        // erase -> write
        block_t_erase(ssd_block);
        ssd_block = tmp_block;

        return ssd_block;
}

ssd_t* ssd_t_write (ssd_t* my_ssd, int PBN) {
        // get the position of page from PBN
        int block_n = (int)(PBN / (PAGE_NUM));
        int page_n = PBN % (PAGE_NUM);

        
        // read
        // block_t* block_tmp = block_t_read(my_ssd, block_n);
        int page_tmp[PAGE_NUM];
        page_read(my_ssd, block_n, page_tmp);

        // erase -> write
        //block_t_write(&my_ssd->block[block_n], block_tmp);
        page_write(my_ssd->block[block_n].page_bitmap, page_tmp);

        // traffic 
        my_ssd->traff_client += 1;
        my_ssd->traff_ftl    += (PAGE_NUM);

        return my_ssd;
}



void destroy_ssd (ssd_t* my_ssd) {
        free(my_ssd);
}




// free block queue
_queue* free_q_init (_queue* q) {
        int i;
        for (i = 1; i <= (int)QUEUE_SIZE; i++) {
                q_push(q, i);
        }
        return q;
}

void* init_mapping_table () {
        int i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                
                mapping_table[i] = -1;
                
        }
        printf("%lld ", mapping_table[0]);
}

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, int LBA) {
        int PBN;

        if (mapping_table[LBA] == -1) {
                PBN = q_pop(free_q);
                mapping_table[LBA] = PBN;
        } 

        PBN = mapping_table[LBA];
        printf("PBN : %d\n", PBN);
        ssd_t_write(my_ssd, PBN);
        
        printf("waf : %f\n", get_WAF(my_ssd));
        return my_ssd;
}
