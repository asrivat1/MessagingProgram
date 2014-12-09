#include "client_map.h"
#include <stdio.h>
#include <stdlib.h>

cmap_node * cmap_init();
void cmap_update(cmap_node * cn, serv_msg * msg, char * pname);
void cmap_del(cmap_node * cn, char * pname);
cmap_node * cmap_get(cmap_node * cn, char * pname);

cmap_node * cmap_init() {
    cmap_node * cn = calloc(1, sizeof(cmap_node));
    if(cn == NULL) {
        perror("ERROR: Malloc sucks \n");
        exit(1);
    }
    cn->next = NULL;
    return cn;
}

void cmap_update(cmap_node * cn, serv_msg * msg, char * pname) {
    cmap_node *curr, *new;
    curr = cn;
    while(curr->next && strcmp(curr->next->pname, pname) < 0) {
        curr = curr->next;
    }
    /*If private name is already there */
    if(curr->next && strcmp(curr->next->pname, pname) == 0) {
        strncpy(curr->next->username, msg->username, 10);
        strncpy(curr->next->room, msg->room, 30);
        return;
    }
    else {
        new = calloc(1, sizeof(cmap_node));
        if(new == NULL) {
            perror("ERROR: Malloc blows \n");
            exit(1);
        }
        strncpy(new->username, msg->username, 10);
        strncpy(new->room, msg->room, 30);
        strncpy(new->pname, pname, MAX_PRIVATE_NAME);
        new->next = curr->next;
        curr->next = new;
    }
    
}

void cmap_del(cmap_node * cn, char * pname) {
    cmap_node *curr, *temp;
    curr = cn;
    while(curr->next && strcmp(curr->next->pname, pname) <= 0){
        if(!strcmp(curr->next->pname, pname)){
            temp = curr->next;
            curr->next = temp->next;
            free(temp); 
        }
    }
}

cmap_node * cmap_get(cmap_node * cn, char * pname) {
    cmap_node *curr;
    curr = cn->next;
    while(curr && strcmp(curr->pname, pname) <= 0){
        if(!strcmp(curr->next->pname, pname)){
            return curr;
        }
    }
    return NULL;
}
