#include "like_list.h"
#include "string.h"
#include <stdio.h>

int strcmp ( const char * str1, const char * str2 );
like_list * like_list_init();
change_mem like_list_update(like_list * ll, serv_msg * msg);
void del_like_list(like_list *ll);

like_list * like_list_init(){
    like_list * l = malloc(sizeof(like_list));
    l_node * sentinal = malloc(sizeof(l_node));
    sentinal->next = NULL;
    sentinal->msg = NULL;
    l->sentinal = sentinal;
    l->num_likes = 0;
    return l;
}

change_mem like_list_update(like_list * ll, serv_msg * msg){
    l_node * telement, * nelement;
    telement = ll->sentinal;
    change_mem change;
    change.change = 0;
    change.msg = NULL;
    /*Traverse list*/
    while(telement->next && strcmp(telement->next->msg->username, msg->username) < 0) {
        telement = telement->next;
    }
    /*If like/unlike for user already exists */
    if(telement->next && strcmp(telement->next->msg->username, msg->username) == 0) {
        if(ltscomp(telement->next->msg->stamp, msg->stamp) == -1){
            if(telement->next->msg->type == LIKE && msg->type == UNLIKE) {
                ll->num_likes --;
                change.change = 1;
            }
            else if(telement->next->msg->type == UNLIKE && msg->type == LIKE) {
                ll->num_likes ++;
                change.change =  1;
            }
            change.msg = telement->next->msg;
            telement->next->msg = msg;
        }
        else
            change.msg = msg;
    }
    else {
        /* Otherwise make new entry */
        nelement = malloc(sizeof(l_node));
        nelement->msg = msg;
        nelement->next = telement->next;
        telement->next = nelement;
        if(msg->type == LIKE) {
            ll->num_likes ++;
            change.change = 1;
        }
    }
    return change;
}


void del_like_list(like_list *ll){
    l_node *temp, *curr;
    curr = ll->sentinal;
    while(curr) {
        temp = curr;
        curr = curr->next;
        if(temp->msg)
            free(temp->msg); 
        free(temp); 
    }
    free(ll);
}
