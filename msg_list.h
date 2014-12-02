#ifndef MSG_LIST
#define MSG_LIST

#include "server_include.h"

/* Node */
typedef struct node{
    serv_msg * msg;
    node * next;
}

/* msg_list */
typedef struct msg_list{
    /*First is a sentinal node at begining */
    node * first;
    /*Last is a sentinal node at end */
    node * last;
}


/* msg list functions */

/* init will create a msg_list and sentinal nodes*/
msg_list * list_init();
/*insert will put a msg into the list*/
/*return 1 if already in*/
short list_insert(msg_list * l, serv_msg * msg);
/*return the lowest time stamp */
int list_lowest(msg_list * l);
/*return the highest time stamp */
int list_highest(msg_list *l);
/*remove the lowest msg in list */
serv_msg * list_pop(msg_list * l);
/*return desired node */
node * list_get(msg_list * l, int num); 
#endif /* MSG_LIST */
