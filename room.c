#include "room.h"

room * room_init(char * name);
void room_insert_msg(room * r, lts l, char * user, char * msg);
void room_insert_like(room * r, lts l, lts like_lts, char* user, short type);


room * room_init(char * name) {
    room * r = malloc(sizeof(room));
    text * t_head = malloc(sizeof(text));
    t_head->next = NULL;
    r->name = name;
    r->t_head = t_head;
    return r; 
}


void room_insert_msg(room * r, lts l, char * user, char * msg){
    text * ctext;
    text * ntext;
    ctext = r->t_head;
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(l, ctext->l_t_s) > 0)
        ctext = ctext->next;
    /*Is it already there? non causal like  */
    if(ltscomp(l, ctext->l_t_s) == 0)
        ctext->l_t_s = l;
        ctext->user = user;
        ctext->msg = msg;
        return;
    ntext = malloc(sizeof(text));
    ntext->l_t_s = l;
    ntext->user = user;
    ntext->msg = msg;
    ntext->likes = like_list_init();
    ntext->next = ctext->next;
    ctext->next = ntext;
}


void room_insert_like(room * r, lts l, lts like_lts, char* user, short type){
    text * ctext;
    text * ntext;
    ctext = r->t_head;
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(l, ctext->l_t_s) > 0)
        ctext = ctext->next;
    if(ltscomp(l, ctext->l_t_s) == 0)
        like_list_update(ctext->likes, like_lts, user, type);
    /* Placeholder msg */
    else {
        ntext = malloc(sizeof(text));
        ntext->l_t_s = l;
        ntext->likes = like_list_init();
        ntext->next = ctext->next;
        ctext->next = ntext;
        like_list_update(ctext->likes, like_lts, user, type);

    }
    return;
}
