#ifndef LIKE_LIST
#define LIKE_LIST

#include "server_include.h"

typedef struct l_node{
    char * user;
    short type;
    lts l_t_s;
    l_node * next;
}l_node;
typedef struct like_list{
    l_node * sentinal;
    int num_likes;
}like_list;

/* Make a new like_list */
like_list * like_list_init();
/* Update like list. Like is 3, unlike is -3 */
void like_list_update(like_list * ll, lts l_t_s, char * user, short type);

#endif /*LIKE_LIST*/
