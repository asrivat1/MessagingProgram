#ifndef LIKE_LIST
#define LIKE_LIST

#include "server_include.h"

typedef struct l_node{
    serv_msg * msg;
    struct l_node * next;
}l_node;

typedef struct like_list{
    l_node * sentinal;
    int num_likes;
}like_list;

/* Make a new like_list */
like_list * like_list_init();
/* Update like list. 
 * Returns 1 if changed
 * Returns 0 if not changed*/
int like_list_update(like_list * ll, serv_msg * msg);

#endif /*LIKE_LIST*/
