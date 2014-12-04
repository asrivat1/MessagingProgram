#include "like_list.h"
#include "string.h"
#include <stdio.h>

int strcmp ( const char * str1, const char * str2 );
like_list * like_list_init();
void like_list_update(like_list * ll, lts l_t_s, char * user, short type);

like_list * like_list_init(){
    like_list * l = malloc(sizeof(like_list));
    l_node * sentinal = malloc(sizeof(l_node));
    sentinal->next = NULL;
    l->sentinal = sentinal;
    l->num_likes = 0;
    return l;
}

void like_list_update(like_list * ll, lts l_t_s, char * user, short type){
    l_node * telement, * nelement;
    telement = ll->sentinal;
    /*Traverse list*/
    while(telement->next != NULL && strcmp(telement->user, user) < 0)
        telement = telement->next;
    /*If like/unlike for user already exists */
    if(strcmp(telement->user, user) == 0) {
        if(telement->l_t_s.index > l_t_s.index)
            return;
        if(telement->l_t_s.index == l_t_s.index) {
            if(telement->l_t_s.server < l_t_s.server) {
                telement->l_t_s = l_t_s;
                /*Like */
                if(type == 3) {
                    if(telement->type != 3) {
                        ll->num_likes++;
                    }
                }
                /*Unlike*/
                else {
                    if(telement->type != -3)
                        ll->num_likes--;
                }
                telement->type = type;
                return;
            }
        }
        else {
            telement->l_t_s = l_t_s;
            /*Like */
            if(type == 3) {
                if(telement->type != 3) {
                    ll->num_likes++;
                }
            }
            /*Unlike*/
            else {
                if(telement->type != -3)
                    ll->num_likes--;
            }
            telement->type = type;
            return;

        }
    }
    /* Otherwise make new entry */
    nelement = malloc(sizeof(l_node));
    if(nelement == NULL){
        fputs("MALLOC ERROR\n", stderr);
    }
    nelement->user = user;
    nelement->type = type;
    nelement->l_t_s = l_t_s;
    nelement->next = telement->next;
    telement->next = nelement;
    if(nelement->type == 3)
        ll->num_likes ++; 
}
