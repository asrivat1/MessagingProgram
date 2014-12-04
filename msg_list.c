#include "msg_list.h"
#include <stdio.h>

msg_list * list_init();
short list_insert(msg_list * l, serv_msg * msg);
int list_lowest(msg_list * l);
int list_highest(msg_list *l);
serv_msg * list_pop(msg_list * l);
node * list_get(msg_list * l, int num);

msg_list * list_init(){
    msg_list * list = malloc(sizeof(msg_list));
    node * first = malloc(sizeof(node));
    if(list == NULL || first == NULL) {
        fputs("MALLOC ERROR\n", stderr);
        return NULL;
    }
    first->msg = NULL;
    first->next = first;
    list->first = first;
    list->last = first;
    return list;
}

short list_insert(msg_list * l, serv_msg * msg){
    node *pelement, *velement;
    int index = msg->stamp.index;

    /*Optimization: see if can add to back (most msgs added to back) */
    if(l->last->msg != NULL && l->last->msg->stamp.index <= index) {
        if(l->last->msg->stamp.index == index)
            return 1;
        velement = malloc(sizeof(node));
        if(velement == NULL) {
            fputs("MALLOC ERROR\n", stderr);
            return 2;
        }
        velement->msg = msg;
        velement->next = l->last->next;
        l->last->next = velement;
        l->last = velement;
    }

    pelement = l->first;
    /*loop through looking for element. Stops when at spot to be inserted */
    while(pelement->next->msg != NULL && pelement->next->msg->stamp.index <=index) {
        if(pelement->next->msg->stamp.index == index)
            return 1;
        pelement = pelement->next;
    }

    /*insert element */
    velement = malloc(sizeof(node));
    if(velement == NULL) {
        fputs("MALLOC ERROR\n", stderr);
        return 2;
    }
    velement->msg = msg;
    velement->next = pelement->next;
    /*Is this the new last element? */
    if(velement->next->msg == NULL) {
        l->last = velement;
    }
    pelement->next = velement;
    return 0;
}


int list_lowest(msg_list * l){
    return (l->first->next->msg != NULL)? l->first->next->msg->stamp.index: -1;
}


int list_highest(msg_list *l){
    return (l->last->msg != NULL)? l->last->msg->stamp.index: -1;
}


serv_msg * list_pop(msg_list * l){
    node * pelement;
    serv_msg * temp;
    if(l->first->next->msg == NULL)
        return NULL;
    if(l->first->next == l->last)
        l->last = l->first;
    pelement = l->first->next;
    temp = pelement->msg;
    l->first->next = pelement->next;
    free(pelement);
    return temp;
}


node * list_get(msg_list * l, int num){
    node *pelement;
    pelement = l->first;
    /*loop through looking for element. Stops when at spot to be inserted */
    while(pelement->next->msg != NULL && pelement->next->msg->stamp.index <= num) {
        if(pelement->next->msg->stamp.index == num)
            return pelement->next;
        pelement = pelement->next;
    }
    return NULL;
}
