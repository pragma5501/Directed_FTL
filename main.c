#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <stdint.h>
#include <regex.h>

#include "FTL.h"


#define BUFF_SIZE 1024

// #define DEBUG 1
#define DEBUG_PRINT 1

ssd_t* my_ssd;
_queue *free_q;

enum TYPE {
        READ  = 0,
        WRITE = 1,
        TRIM  = 3,                
};


int parse (char *text) {
        double time;
        int type;
        int LBA;
        int size;

        if (sscanf(text, "%lf %d %d %d", &time, &type, &LBA, &size) != 5) {
                #ifdef DEBUG
                
                printf("==========\n");
                printf("time : %.9lf\n", time);
                printf("type : %d\n",  type);
                printf("LBA  : %d\n",  LBA / (1024 * 16 ) );
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
                trans_IO_to_ssd(my_ssd, free_q, LBA);
                break;
                
        // trim
        case TRIM:
                break;
        
        default:
                printf("Error : request type is undefined!!\n");
        }
        

}

int read_request (FILE* fp) {
        char buf[BUFF_SIZE];

        int i = 0;

        while (fgets(buf, sizeof(buf), fp)) {
                parse(buf);
                if (i++ >= 20) break;
        }

}

int main (int argc, char** argv) {
        // initialize ssd
        my_ssd = ssd_t_init();

        // initialze mapping table by set value of mapping table -1
        init_mapping_table();

        free_q = q_init();
        free_q = free_q_init(free_q);

        FILE* fp = fopen("./src/intern-trace", "r");
        if( fp == NULL ) {
                printf("Error : File not opened\n");
                return 0;
        }

        read_request(fp);
        fclose(fp);

        get_WAF(my_ssd);

        return 0;
}