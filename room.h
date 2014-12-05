#ifndef ROOM
#define ROOM
/* Room data structure and related functions */

#include "server_include.h"
#include "like_list.h"

typedef struct text{
    serv_msg * msg;
    like_list * likes;
    struct text * next;
}text;
    
typedef struct room{
    char * name;
    text * t_head;
}room;

/*Make a new room */
room * room_init(char * name);
/*Update room with specified msg */
void room_insert(room * r, serv_msg * msg);

#endif /* ROOM */
