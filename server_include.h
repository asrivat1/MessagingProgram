#ifndef SERVER_INCLUDE
#define SERVER_INCLUDE

#include <stdlib.h>

/*Lamport time stamp */
typedef struct lts{
    int server;
    int index;
}lts;

/*Server msg struct */
typedef struct serv_msg{
    lts stamp;
    short type;
    char user[10];
    char room[30];
    char payload[80];
}serv_msg;


/*Function for comparing lamp port time stamps
 *-1 means first is smaller
 * 0 means equal
 * 1 means first is larger */
int ltscomp(lts l1, lts l2);

#endif /* SERVER_INCLUDE */
