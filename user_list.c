#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "user_list.h"

void user_join(user * u, char * joined);
void user_leave(user * u, char * joined);
/*FIXME:    Currently user's that join will only store a name.
 *          It needs room name, so this needs to be fixed.
 *
 *FIXME:    Currently this list does not differentiate between users in
 *          different rooms with the same name.
 *
 *TODO:     Make it more efficient by sorting by name.
 */
void user_join(user * u, char * joined)
{
    /* If list empty, make the first element */
    /* TODO:    Is this section necessary? I think it can be removed,
     *          because if n->next == 0, then the loop will terminate early.
     *          However, problem would still be that user->next could be
     *          null, so the solution would be to start iterating at user,
     *          and check equality at next. */
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
