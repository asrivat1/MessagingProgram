#ifndef ROOM
#define ROOM
/* Room data structure and related functions */

#include "server_include.h"
#include "like_list.h"

typedef struct text{
    lts l_t_s;
    char * msg;
    char * user;
    like_list * likes;
    struct text * next;
}text;
    
typedef struct room{
    char * name;
    text * t_head;
}room;

/*Make a new room */
room * room_init(char * name);
/*Insert a msg */
void room_insert_msg(room * r, lts l, char * user, char * msg);
/*Update like. type 3  means like, type -3  means unlike */
/*l the lamport time stamp of the thing being liked,
 * like_lts is the lamport timestamp of the like*/
void room_insert_like(room * r, lts l, lts like_lts, char * user, short type);

#endif /* ROOM */
