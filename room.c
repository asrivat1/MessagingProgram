#include "room.h"

room * room_init(char * name);
int room_insert_msg(room * r, serv_msg * msg);
change_mem room_insert_like(room *r, serv_msg * msg);


room * room_init(char * name) {
    room * r = malloc(sizeof(room));
    text * t_head = malloc(sizeof(text));
    t_head->next = NULL;
    r->name = name;
    r->t_head = t_head;
    r->recent = t_head;
    r->size = 0;
    return r; 
}

int room_insert_msg(room * r, serv_msg * msg){
    text * ctext;
    text * ntext;
    ctext = r->t_head;
    short pass = 0;
    int change = 0;
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(msg->stamp, ctext->next->msg->stamp) > 0) {
        if(ctext == r->recent)
            pass = 1;
        ctext = ctext->next;
    }
    if(ltscomp(msg->stamp, ctext->next->msg->stamp) == 0) {
        /*If the msg type is DUMMY, then add msg, note change
          if in most recent*/
        if(ctext->next->msg->type == DUMMY) {
            r->size ++;
            if(pass) {
                change = 1;
                if(r->size > 25) {
                    r->recent = r->recent->next;
                }
            }
        }
        /*If this is just a duplicate msg, free the old one.
          This should only happen on client side, since serverside should
          only add msg if it adds to lamport time stamp. Also free dummy */
        free(ctext->next->msg);
        ctext->next->msg = msg;

    }
    /*make new node */
    else {
        ntext = malloc(sizeof(text));
        ntext->next = ctext->next;
        ctext->next = ntext;
        ntext->likes = like_list_init();
        ntext->msg = msg;
        if(pass) {
            change = 1;
            if(r->size > 25)
                r->recent = r->recent->next;
        }
    }
    return change;
}

change_mem room_insert_like(room * r, serv_msg * msg){
    text * ctext;
    text * ntext;
    lts * liked_stamp = (lts *) msg->payload;
    serv_msg * temp;
    short pass = 0;
    change_mem change;
    change.change = 0; 
    change.msg = NULL;
    ctext = r->t_head;
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(*liked_stamp, ctext->next->msg->stamp) > 0) {
        if(ctext == r->recent)
            pass = 1;
        ctext = ctext->next;
    }
    /* msg is in structure-update like list */
    if(ltscomp(*liked_stamp, ctext->next->msg->stamp) == 0) {
            change = like_list_update(ctext->next->likes, msg);
            if(change.change && pass)
                change.change = 1;
            else
                change.change = 0;
    }
    /* like has arrived before msg, make dummy msg */
    else {
        ntext = malloc(sizeof(text));
        ntext->next = ctext->next;
        ctext->next = ntext;
        ntext->likes = like_list_init();
        change = like_list_update(ntext->likes, msg);
        change.change = 0;
        temp = malloc(sizeof(serv_msg));
        temp->stamp = *liked_stamp;
        temp->type = DUMMY;
    }
    return change;
}

/*
void print_room(room * r, int recent) {
    int i = 1;
    text * temp = r->t_head;
    if(recent)
        temp = r->recent->next;
    while(recent) {
        printf(
    }
}
*/
