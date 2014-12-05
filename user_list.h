#ifndef USER_LIST
#define USER_LIST

typedef struct user
{
    char username[10];
    char room[30];
    int instances;
    struct user * next;
}user;

/* User joined a server */
void user_join(user * u, char * user);

/* User leaves a server */
void user_leave(user * u, char * user);

#endif /* USER_LIST */
