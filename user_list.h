#ifndef USER_LIST
#define USER_LIST
#include "server_include.h"

typedef struct user
{
    char username[10];
    char room[30];
    int instances;
    struct user * next;
}user;

/* User joined a server */
void user_join(user * u, serv_msg * user);

/* User leaves a server */
void user_leave(user * u, serv_msg * user);

#endif /* USER_LIST */
