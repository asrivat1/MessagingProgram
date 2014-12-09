#ifndef CLIENT_MAP
#define CLIENT_MAP

#include<sp.h>
#include "server_include.h"
#include <string.h>

typedef struct cmap_node {
    char username[10];
    char room[30];
    char pname[MAX_PRIVATE_NAME];
    struct cmap_node  *next;
}cmap_node;
/* Initiate */
cmap_node * cmap_init();
/* Update */
void cmap_update(cmap_node * cn, serv_msg * msg, char * pname);
/* Delete a private group name entry */
void cmap_del(cmap_node * cn, char * pname);
/* Returns a cmap_node based on private group name */
cmap_node * cmap_get(cmap_node * cn, char * pname);
#endif /* CLIENT_MAP */
