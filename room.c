#include "room.h"
#include "user_list.h"
#include <stdio.h>
#include <string.h>

room * room_init(char * name);
int room_insert_msg(room * r, serv_msg * msg);
change_mem room_insert_like(room *r, serv_msg * msg);
void print_room(room * r, int recent);
void room_update_user(room * r, serv_msg * msg);
void del_room(room * r);
lts get_lts(room * r, int line_num);

room * room_init(char * name) {
    room * r;
    r = malloc(sizeof(room));
    if(!r) {
        perror("MALLOC ERROR\n");
        exit(1);
    }
    text * t_head;
    t_head = calloc(1, sizeof(text));
    if(t_head == 0) {
        perror("MALLOC ERROR\n");
        exit(1);
    }
    t_head->next = NULL;
    r->name = malloc(sizeof(char) * 30);
    if(!r->name) {
        perror("MALLOC ERROR\n");
        exit(1);
    }
    strncpy(r->name, name, 30);
    r->t_head = t_head;
    r->recent = t_head;
    r->size = 0;
    r->users = calloc(1, sizeof(user));
    if(!r->users) {
        perror("MALLOC ERROR\n");
        exit(1);
    }
    r->users->next = NULL;
    r->users->instances = 0;
    return r; 
}

int room_insert_msg(room * r, serv_msg * msg){
    text * ctext;
    text * ntext;
    ctext = r->t_head;
    short pass = 0;
    int change = 0;
    /*Case where recent is in begining */
    if(ctext == r->recent){
        pass = 1;
    }
    /*find correct spot */
    while(ctext->next != NULL && ltscomp(msg->stamp, ctext->next->msg->stamp) > 0) {
        if(ctext == r->recent){
            pass = 1;
        }
        ctext = ctext->next;
    }
    if(ctext->next && ltscomp(msg->stamp, ctext->next->msg->stamp) == 0) {
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
        if(!ntext) {
            perror("MALLOC ERROR\n");
            exit(1);
        }
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
    if(ctext->next && ltscomp(*liked_stamp, ctext->next->msg->stamp) == 0) {
            change = like_list_update(ctext->next->likes, msg);
            if(change.change && pass)
                change.change = 1;
            else
                change.change = 0;
    }
    /* like has arrived before msg, make dummy msg */
    else {
        ntext = malloc(sizeof(text));
        if(!ntext) {
            perror("MALLOC ERROR\n");
            exit(1);
        }
        ntext->next = ctext->next;
        ctext->next = ntext;
        ntext->likes = like_list_init();
        change = like_list_update(ntext->likes, msg);
        change.change = 0;
        temp = malloc(sizeof(serv_msg));
        if(!temp) {
            perror("MALLOC ERROR\n");
            exit(1);
        }
        temp->stamp = *liked_stamp;
        temp->type = DUMMY;
        ntext->msg = temp;
    }
    return change;
}

void print_room(room * r, int recent) {
    int i = 0;
    text * temp = r->t_head->next;
    if(recent)
        temp = r->recent->next;
    printf("Attendees: ");
    user * u = r->users->next; 
    while(u) {
        printf("%s, ", u->username);
        u = u->next;
    }
    printf("\n");
    printf("ROOM: %s \n", r->name);
    while(temp) {
        if(temp->msg->type == DUMMY)
            continue;
        printf("%2d. %s: %-80s \t Likes: %d\n", ++i, temp->msg->username, 
               temp->msg->payload, temp->likes->num_likes);
        temp = temp->next;
    }
}

void room_update_user(room * r, serv_msg * msg) {
    if(msg->type == JOIN)
        user_join(r->users, msg);
    else
        user_leave(r->users, msg);
}

void del_room(room * r) {
    text * curr = r->t_head;
    text * temp;
    user * a, * b;
    while(curr != 0) {
        if(curr->msg != 0) {
            free(curr->msg);
        }
        if(curr->likes != 0)
            del_like_list(curr->likes);
        temp = curr;
        curr = curr->next;
        free(temp);

    }
    /* Free attendees */
    a = r->users;
    while(a != NULL) {
        b = a;
        a = b->next;
        free(b);
    }
    if(r->name != NULL)
        free(r->name);
    free(r);
}

lts get_lts(room * r, int line_num) {
    text * curr = r->recent->next;
    int i = 0;
    while(curr != NULL) {
        if(++i == line_num)
            return curr->msg->stamp;
        curr = curr->next;
    }
    lts fake;
    fake.server = -1;
    fake.index = -1;
    return  fake;
}
