#ifndef LIKE_LIST
#define LIKE_LIST

#include "server_include.h"
/*Struct that contains a short val,
 * change. If change = 1, there was a change in the
 * most recent 25. The like pointer is for client side:
 * If point != NULL, free it. */
typedef struct change_mem{
    short change;
    serv_msg * msg;
}change_mem;

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
 * Returns change = 1 if changed
 * Returns change = 0 if not changed.
 * Any mem that needs to be cleaned up by
 * client is in change_mem.msg*/
change_mem like_list_update(like_list * ll, serv_msg * msg);
/*Delete like list */
void del_like_list(like_list *ll);

#endif /*LIKE_LIST*/
