#ifndef USER_LIST
#define USER_LIST

typedef struct user
{
    char username[10];
    char room[30];
    int instances;
    struct user * next;
}user;

typedef struct user_list
{
    user * users[5];
}user_list;

/* User joined a server */
void user_join(user_list * ul, int server, char * user);

/* User leaves a server */
void user_leave(user_list * ul, int server, char * user);

/* Server leaves group */
void clear_server(user_list * ul, int server);

#endif /* USER_LIST */
