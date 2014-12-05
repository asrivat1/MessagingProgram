#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "user_list.h"

void user_join(user * u, char * joined);
void user_leave(user * u, char * joined);

void user_join(user * u, char * joined)
{
    /* If list empty, make the first element */
    if(u->next == 0)
    {
        u->next = malloc(sizeof(user));
        sprintf(u->next->username, "%s", joined);
        u->next->instances = 1;
        return;
    }

    user * current = u->next;

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

void user_leave(user * u, char * joined)
{
    user * current = u;

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
