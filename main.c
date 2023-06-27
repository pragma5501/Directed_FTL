#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <stdint.h>
#include <regex.h>

#include "FTL.h"


#define BUFF_SIZE 1024

#define DEBUG 1
// #define DEBUG_PRINT 1



enum TYPE {
        READ  = 0,
        WRITE = 1,
        TRIM  = 3,                
};


int parse (char *text, ssd_t* my_ssd, _queue* free_q) {
        double time;
        int type;
        int LBA;
        int size;

        if (sscanf(text, "%lf %d %d %d", &time, &type, &LBA, &size) != 5) {
                #ifdef DEBUG
                
                printf("==========\n");
                printf("time : %.9lf\n", time);
                printf("type : %d\n",  type);
                printf("LBA  : %d\n",  LBA );
                printf("size : %d\n",  size);
                printf("==========\n");
                #endif
        }

        switch (type) {
        // read
        case READ:
                break;        

        // write
        case WRITE:
                printf("write request\n");
                trans_IO_to_ssd(my_ssd, free_q, LBA);

                break;
                
        // trim
        case TRIM:
                break;
        
        default:
                printf("Error : request type is undefined!!\n");
                break;
        }
        

}

int read_request (FILE* fp, ssd_t* my_ssd, _queue* free_q) {
        char buf[BUFF_SIZE];

        int i = 0;

        while (fgets(buf, sizeof(buf), fp)) {
                parse(buf, my_ssd, free_q);
                if (i++ >= 20) break;
        }

}

int main (int argc, char** argv) {
        // initialize ssd
        ssd_t* my_ssd;
        my_ssd = ssd_t_init();

        
        // initialze mapping table by set value of mapping table -1
        init_mapping_table();

        _queue* free_q = q_init();
        free_q = free_q_init(free_q);

        FILE* fp = fopen("./src/intern-trace", "r");
        if( fp == NULL ) {
                printf("Error : File not opened\n");
                return 0;
        }

        #ifdef DEBUG
        printf("send request\n");
        #endif 
        read_request(fp, my_ssd, free_q);
        fclose(fp);


        destroy_ssd(my_ssd);
        q_destroy(free_q);

        return 0;
}