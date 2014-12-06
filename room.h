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

/*TODO: add attendees */    
typedef struct room{
    char * name;
    text * t_head;
    text * recent;
    int size;
}room;

/*Make a new room */
room * room_init(char * name);
/*Update room with specified msg */
int room_insert_msg(room * r, serv_msg * msg);
/*Update room with specified like */
change_mem room_insert_like(room * r, serv_msg * msg);
/*Print room */
void print_room(room * r, int recent);
/*Delete room*/
void del_room(room * r);
#endif /* ROOM */
