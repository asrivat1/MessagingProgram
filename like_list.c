#include "like_list.h"
#include "string.h"
#include <stdio.h>

int strcmp ( const char * str1, const char * str2 );
like_list * like_list_init();
int like_list_update(like_list * ll, serv_msg * msg);

like_list * like_list_init(){
    like_list * l = malloc(sizeof(like_list));
    l_node * sentinal = malloc(sizeof(l_node));
    sentinal->next = NULL;
    sentinal->msg = NULL;
    l->sentinal = sentinal;
    l->num_likes = 0;
    return l;
}

int like_list_update(like_list * ll, serv_msg * msg){
    l_node * telement, * nelement;
    telement = ll->sentinal;
    short change = 0;
    /*Traverse list*/
    while(telement->next != NULL && strcmp(telement->msg->user, msg->user) < 0)
        telement = telement->next;
    /*If like/unlike for user already exists */
    if(strcmp(telement->msg->user, msg->user) == 0) {
        if(ltscomp(telement->msg->stamp, msg->stamp) == -1){
            if(telement->msg->type == LIKE && msg->type == UNLIKE) {
                ll->num_likes --;
                change = 1;
            }
            else if(telement->msg->type == UNLIKE && msg->type == LIKE) {
                ll->num_likes ++;
                change = 1;
            }
            telement->msg = msg;
        }
    }
    else {
        /* Otherwise make new entry */
        nelement = malloc(sizeof(l_node));
        nelement->msg = msg;
        nelement->next = telement->next;
        telement->next = nelement;
        if(msg->type == LIKE) {
            ll->num_likes ++;
            change = 1;
        }
    }
    return change;
}
