#ifndef ROOM_LIST
#define ROOM_LIST
#include "server_include.h"
#include "room.h"
typedef struct room_node{
    room * r;
    struct room_node * next;
}room_node;

room_node * room_list_init();
void room_list_update(room_node * r, serv_msg * msg);
room * room_list_get_room(room_node * r, char * room_name);
#endif /* ROOM_LIST */
