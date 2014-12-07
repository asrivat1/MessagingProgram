#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "user_list.h"

void user_join(user * u, serv_msg * joined);
void user_leave(user * u, serv_msg * joined);

void user_join(user * u, serv_msg * joined)
{
    if(u->next == NULL)
    {
        u->next = calloc(1, sizeof(user));
        sprintf(u->next->username, "%s", joined->username);
        sprintf(u->next->room, "%s", joined->room);
        u->next->instances = 1;
        return;
    }

    user * current = u;

    /* Iterate through list */
    while(current->next != 0)
    {
        /* If we find that user */
        if(strcmp(current->next->username, joined->username) == 0 &&
           strcmp(current->next->room, joined->room) == 0)
        {
            current->instances++;
            return;
        }

        /* Step */
        current = current->next;
    }

    /* We didn't find it */
    current->next = calloc(1, sizeof(user));
    sprintf(current->next->username, "%s", joined->username);
    sprintf(current->next->room, "%s", joined->room);
    current->next->instances = 1;
    current->next->next = NULL;
}

void user_leave(user * u, serv_msg * joined)
{
    user * current = u;

    /* Iterate through list */
    while(current->next != 0)
    {
        /* If we find that user */
        if(strcmp(current->next->username, joined->username) == 0 &&
           strcmp(current->next->room, joined->room) == 0 )
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
