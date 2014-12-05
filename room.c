#include "room.h"

room * room_init(char * name);
void room_insert(room * r, serv_msg * msg);


room * room_init(char * name) {
    room * r = malloc(sizeof(room));
    text * t_head = malloc(sizeof(text));
    t_head->next = NULL;
    r->name = name;
    r->t_head = t_head;
    return r; 
}


void room_insert_msg(room * r, serv_msg * msg){
    text * ctext;
    text * ntext;
    ctext = r->t_head;
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(msg->stamp, ctext->msg->stamp) > 0)
        ctext = ctext->next;
    if(ltscomp(msg->stamp, ctext->msg->stamp) == 0){
        /*If the msg type is a msg, then a dummy
          msg is there */
        if(msg->type == MSG) {
            free(ctext->msg);
            ctext->msg = msg;
        }
        else 
            like_list_update(ctext->likes, msg);
        return;
    }
    ntext = malloc(sizeof(text));
    ntext->next = ctext->next;
    ctext->next = ntext;
    ntext->likes = like_list_init();
    if(msg->type == MSG)
        ntext->msg = msg;
    else
        like_list_update(ntext->likes, msg);
    return;
}

