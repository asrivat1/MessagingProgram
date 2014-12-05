#ifndef SERVER_INCLUDE
#define SERVER_INCLUDE

#include <stdlib.h>

#define JOIN 1
#define MSG 2
#define LIKE 3
#define UNLIKE -3
#define LEAVE -1

/*Lamport time stamp */
typedef struct lts{
    int server;
    int index;
}lts;

/*Server msg struct */
typedef struct serv_msg{
    short type;
    lts stamp;
    char username[10];
    char room[30];
    char payload[80];
}serv_msg;


/*Function for comparing lamp port time stamps
 *-1 means first is smaller
 * 0 means equal
 * 1 means first is larger */
int ltscomp(lts l1, lts l2);

#endif /* SERVER_INCLUDE */
