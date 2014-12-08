#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sp.h>
#include<sys/time.h>

#include "lamp_struct.h"
#include "server_include.h"
#include "user_list.h"
#include "room_list.h"

#define MAX_MEMBERS  100
#define MAX_MESSLEN  102400
#define NUM_SERVERS 5

static mailbox Mbox;
static char Private_group[MAX_GROUP_NAME];
lamp_struct * messages;
room_node * rooms;
lts * lamport_time;
char server_group[] = "Servers";
char User[] = "Server#";
char spread_name[] = "10210";
user * users[NUM_SERVERS];
int ret;
FILE *fd;
int proc_index;
serv_msg * msg_rec;
lts * max[NUM_SERVERS];
int max_sender[NUM_SERVERS];
lts * min[NUM_SERVERS];
int num_merged = 0;
int num_lts = 0;

int group_status[NUM_SERVERS];
int prev_group_status[NUM_SERVERS];

void handle_input(int argc, char *argv[]);
void Read_message();
void handleMessage(serv_msg * msg_buf, char * sender, char target_groups[MAX_MEMBERS][MAX_GROUP_NAME],
        int service_type, int num_groups);
void merge_messages();
void storeMessage(serv_msg * msg_buf);
void merge();
void sendToClient(serv_msg * msg_buf);
void send_room(char * client_group, char * rm);
void checkError(char * action);
void clear_server(int server);
void sendFakeMsg();

int main(int argc, char *argv[])
{
    /* Allocate memory and handle input */
    msg_rec = malloc(sizeof(serv_msg));
    if(msg_rec == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }

    int i;
    for(i = 0; i < NUM_SERVERS; i++)
    {
        users[i] = malloc(sizeof(user));
        if(users[i] == 0)
        {
            perror("MALLOC HATES ME\n");
            exit(1);
        }
        max[i] = malloc(sizeof(lts));
        if(max[i] == 0)
        {
            perror("MALLOC HATES ME\n");
            exit(1);
        }
        min[i] = malloc(sizeof(lts));
        if(min[i] == 0)
        {
            perror("MALLOC HATES ME\n");
            exit(1);
        }
    }

    messages = lamp_struct_init();
    
    handle_input(argc, argv);

    rooms = room_list_init();

    /* Set up LTS data structure */
    lamport_time = malloc(sizeof(lts));
    if(lamport_time == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }
    lamport_time->server = proc_index;
    lamport_time->index = 1;

    sp_time test_timeout;
    test_timeout.sec = 5;
    test_timeout.usec = 0;

    fd = fopen(User, "r");
    serv_msg * read_buf = malloc(sizeof(serv_msg));
    size_t s = 1;
    while(s == 1)
    {
        s = (fd != 0) ? fread(read_buf, sizeof(serv_msg), 1, fd) : 0;
        if(s == 1)
        {
            storeMessage(read_buf);
            printf("Reading msg from Server%d Index%d\n", read_buf->stamp.server, read_buf->stamp.index);
            printf("Type is %d\n", read_buf->type);
            printf("%s\n", read_buf->payload);
            room_list_update(rooms, read_buf);
            read_buf = malloc(sizeof(serv_msg));
            if(!read_buf)
            {
                perror("Error with malloc\n");
                exit(1);
            }
        }
    }
    if(fd != 0)
        fclose(fd);
    

    /* Connect and join various groups */
	ret = SP_connect_timeout( spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
    checkError("Connect");
    ret = SP_join(Mbox, server_group);
    checkError("Join");
    ret = SP_join(Mbox, User);
    checkError("Join");
    char server_client[MAX_GROUP_NAME];
    sprintf(server_client, "Server%d-Client", proc_index);
    ret = SP_join(Mbox, server_client);
    checkError("Join");


    /* Set up E - DO NOT MOVE */
	E_init();
    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );
	E_handle_events();

    return 0;
}

void handleMessage(serv_msg * msg_buf, char * sender, char target_groups[MAX_MEMBERS][MAX_GROUP_NAME],
        int service_type, int num_groups)
{
    int i;
    lts payload_lts[NUM_SERVERS];

    if( Is_regular_mess( service_type ) )
    {
        printf("\nGot a regular message sent to %s of type %d\n", target_groups[0], msg_buf->type);

        /* Ignore if from self unless LTS array */
        if(!strcmp(sender, Private_group) && msg_buf->type != 4)
        {
            printf("It was from myself\n");
            return;
        }


        msg_rec = malloc(sizeof(serv_msg));
        if(!msg_rec)
        {
            perror("MALLOC NOT WORKING\n");
            exit(1);
        }
        memcpy(msg_rec, msg_buf, sizeof(serv_msg));

        /* If from another server */
        if(!strcmp(target_groups[0], server_group))
        {
            /* Put it in the room */
            room_list_update(rooms, msg_rec);

            /* If it's not a server only message and we don't already have it */
            if((msg_buf->type != 4) && msg_buf->type != 1 && msg_buf->type != -1
                    && ltscomp(msg_buf->stamp, lamp_array(messages)[atoi(&sender[7])]) == 1)
            {
                storeMessage(msg_buf);

                /* Write to file */
                fd = fopen(User, "a");
                fwrite(msg_rec, sizeof(serv_msg), 1, fd);
                fclose(fd);
            }

            /* Handle join/leave messages */
            switch(msg_buf->type)
            {
                /* Join */
                case 1:
                    printf("\nNew user %s\n", msg_buf->username);
                    /* Upon first joining a room, we must send the history */
                    user_join(users[atoi(&sender[7])], msg_buf);
                    break;
                /* Leave */
                case -1:
                    printf("\nUser %s has left\n", msg_buf->username);
                    user_leave(users[atoi(&sender[7])], msg_buf);
                    break;
                /* Merge */
                case 4:
                    num_lts++;
                    /* Retreive the array of LTS */
                    memcpy(payload_lts, msg_buf->payload, sizeof(lts) * 5);
                    /* See if this is has a max or min */
                    for(i = 0; i < NUM_SERVERS; i++)
                    {
                        if(max[i] == 0 || ltscomp(payload_lts[i], *max[i]) == 1
                                || (ltscomp(payload_lts[i], *max[i]) == 0 && atoi(&sender[7]) < max_sender[i]))
                        {
                            if(max[i] == 0)
                            {
                                max[i] = malloc(sizeof(lts));
                                if(!max[i])
                                {
                                    perror("MALLOC NOT WORKING\n");
                                    exit(1);
                                }
                            }
                            max[i]->server = payload_lts[i].server;
                            max[i]->index = payload_lts[i].index;
                            max_sender[i] = atoi(&sender[7]);
                        }
                        if(min[i] == 0 || ltscomp(payload_lts[i], *min[i]) == -1)
                        {
                            if(min[i] == 0)
                            {
                                min[i] = malloc(sizeof(lts));
                                if(!min[i])
                                {
                                    perror("MALLOC NOT WORKING\n");
                                    exit(1);
                                }
                            }
                            min[i]->server = payload_lts[i].server;
                            min[i]->index = payload_lts[i].index;
                        }
                    }
                    /* If I have info from all servers, send messages */
                    if(num_merged == num_lts)
                    {
                        num_lts = 0;
                        merge_messages();
                    }
                    break;
            }
        }
        /* Otherwise it's from client */
        else
        {
            /* Increment LTS */
            lamport_time->index++;
            msg_buf->stamp.index = lamport_time->index;
            msg_buf->stamp.server = proc_index;
            msg_rec->stamp.index = lamport_time->index;
            msg_rec->stamp.server = proc_index;

            if(msg_buf->type != 1 && msg_buf->type != -1 && msg_buf->type != 5)
            {
                /* Write to file */
                fd = fopen(User, "a");
                fwrite(msg_buf, sizeof(serv_msg), 1, fd);
                fclose(fd);
                lamp_struct_insert(messages, msg_rec);
            }

            if(msg_buf->type != 5)
            {
                room_list_update(rooms, msg_rec);
            }

            /* If it's a view, send that */
            if(msg_buf->type == VIEW)
            {
                for(i = 0; i < NUM_SERVERS; i++)
                {
                    sprintf(msg_rec->payload + i, "%d", group_status[i]);
                }
                printf("Sending view %s\n", msg_rec->payload);
                ret = SP_multicast(Mbox, SAFE_MESS, sender, 2, sizeof(serv_msg), (char *) msg_rec);
                checkError("Multicast");
            }

            /* If it's a join, send all info */
            if(msg_buf->type == 1)
            {
                send_room(sender, msg_buf->room);
            }

            if(msg_buf->type != 5)
            {
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_buf);
                checkError("Multicast");
            }
        }
        /* Send message to client unless it's an LTS array */
        if(msg_buf->type != 4 && msg_buf->type != 5)
        {
            sendToClient(msg_buf);
        }
    }
    else if( Is_reg_memb_mess(service_type))
    {
        /* If message from server_group */
        if (!strcmp(sender, server_group))
        {
            int server_index;
            int merge_case = 0;
            printf("\nServer group membership change.\n");
            for (i=0; i < NUM_SERVERS; i++)
            {
                prev_group_status[i] = (i != proc_index) ? group_status[i] : 1;
                group_status[i] = 0;
            }
            for (i=0 ; i < num_groups; i++)
            {
                /* You can print out the private group for each server if you want */
                printf("%s\n", &target_groups[i][0]);
                /* You can get the index of the server by reading in
                 * an integer starting at the second character (since the first is '#') */
                server_index = atoi(&target_groups[i][7]);
                group_status[server_index] = 1;
                if (!prev_group_status[server_index])
                {
                    merge_case = 1;
                }
            }
            for(i = 0; i < NUM_SERVERS; i++)
            {
                /* If a server left */
                if(i != proc_index && prev_group_status[i] > group_status[i])
                {
                    printf("Clearing server %d\n", proc_index);
                    clear_server(i);
                }
            }
            if (merge_case)
            {
                /* Deal with it */
                printf("\nMerging! there are %d groups\n", num_groups);
                num_merged = num_groups;
                merge();
            }
        }
    }
}
void storeMessage(serv_msg * msg_buf)
{
    /* Allocate new memory for storage */
    msg_rec = malloc(sizeof(serv_msg));
    if(msg_rec == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }
    memcpy(msg_rec, msg_buf, sizeof(serv_msg));

    /* Increment LTS */
    if( ltscomp(msg_rec->stamp, *lamport_time) == 1 )
    {
        lamport_time->index = 1 + msg_rec->stamp.index;
    }
    else
    {
        lamport_time->index = 1 + lamport_time->index;
    }

    /* Add to list of messages and handle */
    lamp_struct_insert(messages, msg_rec);
}

void sendToClient(serv_msg * msg_buf)
{
    char client_group[MAX_GROUP_NAME];

    printf("%s\n", msg_buf->payload);
    /* Send to the client group */
    sprintf(client_group, "%s-%s", msg_buf->room, User);
    ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) msg_buf);
    checkError("Multicast");
}

void Read_message()
{
    static char in_mess[MAX_MESSLEN];
    char sender[MAX_GROUP_NAME];
    char target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
    int service_type = 0;
    int endian_mismatch;
    int16 mess_type;
    membership_info memb_info;\
    int num_groups;

    serv_msg * msg_buf = malloc(sizeof(serv_msg));
    if(msg_buf == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }

    /* Receive messages */
    ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups,
                      &mess_type, &endian_mismatch, sizeof(serv_msg), (char *) msg_buf );
    checkError("Receive");
    ret = SP_get_memb_info( in_mess, service_type, &memb_info );

    /* Handle the message */
    handleMessage(msg_buf, sender, target_groups, service_type, num_groups);

}

void merge_messages()
{
    int i;
    int j;
    serv_msg * msg_send = malloc(sizeof(serv_msg));
    if(msg_send == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }

    printf("\nMerging messages\n");

    for(i = 0; i < NUM_SERVERS; i++)
    {
        /* If I have the max LTS for that server */
        printf("Max sender is %d\n", max_sender[i]);
        if(max_sender[i] == proc_index)
        {
            printf("I have max for Server%d\n", i);
            /* Send everything from min to max */
            for(j = 0; j < messages->s_list[i]->size; j++)
            {
                if(min[i] != 0 && messages->s_list[i]->arr[j]->stamp.index >= min[i]->index)
                {
                    memcpy(msg_send, messages->s_list[i]->arr[j], sizeof(serv_msg));
                    printf("Sending %s\n", msg_send->payload);
                    ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_send);
                    checkError("Multicast");
                }
                else if(min[i] == 0)
                {
                    memcpy(msg_send, messages->s_list[i]->arr[j], sizeof(serv_msg));
                    printf("Sending %s\n", msg_send->payload);
                    ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_send);
                    checkError("Multicast");
                }
            }
        }
    }
    
    free(msg_send);
    msg_send = 0;
}

void merge()
{
    int i;
    lts * server_lts;
    serv_msg * msg_send = malloc(sizeof(serv_msg));
    if(msg_send == 0)
    {
        perror("MALLOC HATES ME\n");
        exit(1);
    }

    /* Clear previous max/min */
    for(i = 0; i < NUM_SERVERS; i++)
    {
        max_sender[i] = -1;
        free(max[i]);
        max[i] = 0;
        free(min[i]);
        min[i] = 0;
    }

    /* Send my LTS for each server */
    server_lts = lamp_array(messages);
    memcpy(msg_send->payload, server_lts, sizeof(lts) * 5);
    msg_send->type = 4;
    ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_send);
    checkError("Multicast");

    /* Clear my list of users for everyone else */
    for(i = 0; i < NUM_SERVERS; i++)
    {
        if(i != proc_index)
        {
            clear_server(i);
        }
    }

    /* Send out my users */
    printf("Sending my users\n");
    user * current = users[proc_index]->next;
    while(current != 0)
    {
        printf("Sending my user %s\n", msg_send->username);
        msg_send->type = 1;
        sprintf(msg_send->username, "%s", current->username);
        sprintf(msg_send->room, "%s", current->room);
        for(i = 0; i < current->instances; i++)
        {
            ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_send);
            checkError("Multicast");
        }
        current = current->next;
    }
}

void handle_input(int argc, char * argv[]) {
    if(argc < 1) {
        perror("Mcast: Argument Error");
        exit(1);
    }
    
    /* Get server ID */
    sscanf(argv[1], "%d", &proc_index);
    User[6] = proc_index + 48;
    printf("I am %s\n", User);
}

void clear_server(int server)
{
    int i;
    serv_msg * send_msg = malloc(sizeof(serv_msg));
    char client_group[MAX_GROUP_NAME];
    if(send_msg == 0)
    {
        perror("MALLOC DID NOT WORK :(\n");
        exit(1);
    }

    send_msg->type = -1;

    user * current = users[server]->next;
    while(current != 0)
    {
        printf("Removing user %s\n", current->username);
        user * tmp = current;
        strcpy(send_msg->username, tmp->username);
        strcpy(send_msg->room, tmp->room);

        for(i = 0; i < tmp->instances; i++)
        {
            sprintf(client_group, "%s-%s", tmp->room, User);
            ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) send_msg);
            room_list_update(rooms, send_msg);
            checkError("Multicast");
        }

        free(tmp);
        tmp = 0;

        current = current->next;
    }
}
/* send room to client */
void send_room(char * client_group, char * rm) {
    int i;

    serv_msg * send_msg = malloc(sizeof(serv_msg));
    if(send_msg == 0)
    {
        perror("MALLOC DID NOT WORK :(\n");
        exit(1);
    }
    send_msg->type = 1;
    room * r = room_list_get_room(rooms, rm);
    text * t = r->t_head->next;
    l_node * l;
    while(t) {
        /*SEND t->msg */
        printf("Sending an old message to client\n");
        ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) t->msg);
        checkError("Multicast");
        l = t->likes->sentinal->next;
        while(l) {
            /*SEND l->msg */
            ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) l->msg);
            checkError("Multicast");
            l = l->next;
        }
        t = t->next;
    }
    /* Send attendee info */ 
    user * curr_user = r->users;
    while(curr_user != 0)
    {
        /* Send a join for that user */
        strcpy(send_msg->username, curr_user->username);
        strcpy(send_msg->room, rm);
        for(i = 0; i < curr_user->instances; i++)
        {
            printf("Sending %s\n", curr_user->username);
            ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) send_msg);
            checkError("Multicast");
        }
        curr_user = curr_user->next;
    }
}

void checkError(char * action) {
    if (ret < 0)
    {
        printf("Error performing %s\n", action);
        exit(1);
    }
}

