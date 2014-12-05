#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "user_list.h"

user_list * user_list_init();
void user_join(user_list * ul, int server, char * user);
void user_leave(user_list * ul, int server, char * user);
void clear_server(user_list * ul, int server);

user_list * user_list_init()
{
    int i;
    user_list * ul = malloc(sizeof(user_list));
    for(i = 0; i < 5; i++)
    {
        ul->users[i] = malloc(sizeof(user));
    }
    return ul;
}

void clear_server(user_list * ul, int server)
{
    user * current = ul->users[server]->next;
    while(current != 0)
    {
        user * tmp = current;
        current = current->next;
        free(tmp);
    }
}

void user_join(user_list * ul, int server, char * joined)
{
    /* If list empty, make the first element */
    if(ul->users[server]->next == 0)
    {
        ul->users[server]->next = malloc(sizeof(user));
        sprintf(ul->users[server]->next->username, "%s", joined);
        ul->users[server]->next->instances = 1;
        return;
    }

    user * current = ul->users[server]->next;

    /* Iterate through list */
    while(current->next != 0)
    {
        /* If we find that user */
        if(strcmp(current->username, joined) == 0)
        {
            current->instances++;
            return;
        }

        /* Step */
        current = current->next;
    }
    if(strcmp(current->username, joined) == 0)
    {
        current->instances++;
        return;
    }

    /* We didn't find it */
    current->next = malloc(sizeof(user));
    sprintf(current->next->username, "%s", joined);
    current->next->instances = 1;
}

void user_leave(user_list * ul, int server, char * joined)
{
    user * current = ul->users[server];

    /* Iterate through list */
    while(current->next != 0)
    {
        /* If we find that user */
        if(strcmp(current->next->username, joined) == 0)
        {
            /* Remove the user if no instances */
            if(--current->next->instances == 0)
            {
                user * tmp = current->next;
                current->next = tmp->next;
                free(tmp);
            }
            return;
        }

        /* Step */
        current = current->next;
    }
}
