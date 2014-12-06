#include "room_list.h"
#include "string.h"

room_node * room_list_init();
void room_list_update(room_node * r, serv_msg * msg);
room * room_list_get_room(room_node * r, char * room_name);



room_node * room_list_init() {
    room_node * r = malloc(sizeof(room_node));
    r->next = NULL;
    r->r = NULL;
    return r;
}


void room_list_update(room_node * r, serv_msg * msg){
    room_node * curr = r;
    room_node * temp;
    while(curr->next != NULL && strcmp(curr->next->r->name, msg->room) < 0){
        curr = curr->next;
    }
    /*Room needs to be made */
    if(curr->next == NULL || strcmp(curr->next->r->name, msg->room) != 0) {
        printf("Room Created\n");
        temp = malloc(sizeof(room_node));
        temp->r = room_init(msg->room);
        temp->next = curr->next;
        curr->next = temp;
    }
    /*Add msg to room*/
    if(msg->type == MSG)
        room_insert_msg(curr->next->r, msg);
    /*Add like to room*/
    else if(msg->type == LIKE || msg->type == UNLIKE)
        room_insert_like(curr->next->r, msg);
    /*Add user to room */
    else if(msg->type == JOIN || msg->type == LEAVE)
        room_update_user(curr->next->r, msg);
}

room * room_list_get_room(room_node * r, char * room_name) {
    room_node * curr = r->next;
    while(curr != NULL) {
        if(!strcmp(curr->r->name, room_name))
            return curr->r;
        curr = curr->next;
    }
    return NULL;
}
