#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sp.h>
#include<sys/time.h>

#include "lamp_struct.h"
#include "server_include.h"
#include "room.h"

#define MAX_MEMBERS  100
#define MAX_MESSLEN  102400
#define NUM_SERVERS 5

static mailbox Mbox;
static char Private_group[MAX_GROUP_NAME];
lamp_struct * messages;
lts * lamport_time;
char * room[];
char server_group[7];
char server_room_group[80];
char * User;
char spread_name[] = "10210";
int ret;
FILE *fd;
int num_sent = 1;
int proc_index;
int sent_done = 0;
double time;
serv_msg * msg_send;
serv_msg * msg_rec;
struct timeval val;

int group_status[NUM_SERVERS];
int prev_group_status[NUM_SERVERS];

void handle_input(int argc, char *argv[]);
void Read_message();
void read_input();
void handle_message(serv_msg * msg);
void checkError(char * action);

int main(int argc, char *argv[])
{
    /* Allocate memory and handle input */
    msg_send = malloc(sizeof(serv_msg));
    msg_rec = malloc(sizeof(serv_msg));

    messages = lamp_struct_init();
    
    /* Get client ID */
    gettimeofday(&val, NULL);
    sprintf(User, "%ld\n", val.tv_sec);

    sp_time test_timeout;
    test_timeout.sec = 5;
    test_timeout.usec = 0;

    /* Connect and join various groups */
	ret = SP_connect_timeout( spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
    checkError("Connect");

    /* Set up E - DO NOT MOVE */
	E_init();
    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );
	E_handle_events();

    read_input();

    return 0;
}

void read_input()
{
    char command[80];

    while(1)
    {
        printf("> ");
        scanf("%79s", command);

        switch(command[0])
        {
            /* Login user */
            case 'u':
                break;
            /* Connect to server */
            case 'c':
                proc_index = command[2];
                strprintf(server_group, "Server%d", proc_index);
                ret = SP_join(Mbox, "Server%d", proc_index);
                break;
            /* Connect to room */
            case 'j':
                strprintf(room, "%s", command + 2);
                strprintf(server_room_group, "%s-Server%d", command + 2, proc_index);
                ret = SP_join(Mbox, server_room_group);
            /* Append message */
            case 'a':
                strprintf(msg_send->room, "%s", room);
                strprintf(msg_send->payload, "%s", command + 2);
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
                break;
            /* Like a message */
            case 'l':
                break;
            /* Remove a like */
            case 'r':
                break;
            /* View full history */
            case 'h':
                break;
            /* View other servers */
            case 'v':
                break;
        }
    }
}

void Read_message()
{
    static char in_mess[MAX_MESSLEN];
    char sender[MAX_GROUP_NAME];
    char target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
    char client_group[MAX_GROUP_NAME];
    int num_groups;
    int service_type = 0;
    int endian_mismatch;
    int16 mess_type;
    membership_info memb_info;

    /* Receive messages */
    ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups,
                      &mess_type, &endian_mismatch, sizeof(serv_msg), (char *) msg_rec );
    ret = SP_get_memb_info( in_mess, service_type, &memb_info );

    if( Is_regular_mess( service_type ) )
    {
        printf("Got a regular message\n");

        /* If about my room */
        if(!strcmp(target_groups[0], server_room_group))
        {
            /* Handle message */
        }
    }
}

void checkError(char * action) {
    if (ret < 0)
    {
        printf("Error performing %s\n", action);
    }
}
