#include "server_include.h"

int ltscomp(lts l1, lts l2);


int ltscomp(lts l1, lts l2){
    if(l1.index > l2.index)
        return 1;
    if(l1.index == l2.index) {
        if(l1.server > l2.server)
            return 1;
        if(l1.server == l2.server)
            return 0;
    }
    return -1;
}


