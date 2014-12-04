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
}serv_msg;
#endif /* SERVER_INCLUDE */
