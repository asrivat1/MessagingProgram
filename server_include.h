#ifndef SERVER_INCLUDE
#define SERVER_INCLUDE
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
    char * payload;
    /*You can not hold strings in a struct, so payload has to be malloced.
     *When a msg is sent, payload needs to be sent too. */
}serv_msg;


/*Function for comparing lamp port time stamps
 *-1 means first is smaller
 * 0 means equal
 * 1 means first is larger */
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


#endif /* SERVER_INCLUDE */
