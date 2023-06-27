#include <stdio.h>
#include "ssd.h"

double get_WAF (ssd_t* my_ssd) {
        double WAF = my_ssd->traff_ftl / my_ssd->traff_client;
        return WAF;
}