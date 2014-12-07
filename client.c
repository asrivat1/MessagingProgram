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
lts like_lts;
char chatroom[30];
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
char username[12];
int in_room = 0;
int con_server = 0;
room * m_room;
char r_name[30];

int group_status[NUM_SERVERS];
int prev_group_status[NUM_SERVERS];

void handle_input(int argc, char *argv[]);
void Read_message();
void Read_input();
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

    printf(">");

    /* Set up E - DO NOT MOVE */
	E_init();
    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );
    E_attach_fd( 0, READ_FD, Read_input, 0, NULL, LOW_PRIORITY );
	E_handle_events();


    return 0;
}

void Read_input()
{
    int i;
    char command[80];
    char server_client[14];

    for(i = 0; i < sizeof(command); i++)
        command[i] = 0;
    if( fgets(command, 80, stdin) == NULL) {
        perror("FGETS ERROR\n");
        exit(1);
    }

    switch(command[0])
    {
        /* Login user */
        case 'u':
            if(in_room) {
                /*Send Leave msg*/
                sprintf(msg_send->username, "%s", username);
                sprintf(msg_send->room, "%s", chatroom);
                msg_send->type = LEAVE;
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
 
                /*Delete room data structure */
                printf("Logged out. \n");
                del_room(m_room);
                m_room = NULL;
                in_room = 0;
            } 
            for(i = 0; i < sizeof(username); i++)
                username[i] = 0;
            sprintf(username, "%s", command + 2);
            printf("Logged in as %s. \n", username);
            break;
        /* Connect to server */
        case 'c':
            if(con_server != 1) {
                printf("Logging out from old server\n");
                if(in_room) {
                    /*Send Leave msg */
                    sprintf(msg_send->username, "%s", username);
                    sprintf(msg_send->room, "%s", chatroom);
                    msg_send->type = LEAVE;
                    ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
 
                    /*Delete room data structure */
                    del_room(m_room);
                    m_room = NULL;

                }
                /*Leave Server*/
                SP_leave(Mbox, server_client);
            }
            proc_index = atoi(command + 2);
            sprintf(server_group, "Server%d", proc_index);
            sprintf(server_client, "Server%d-Client", proc_index);
            ret = SP_join(Mbox, server_client);
            con_server = 1;
            break;
        /* Connect to room */
        case 'j':
            if(username[0] != 0) {
                printf("No Username. \n");
                break;
            }
            if(in_room) {
                /*Send Leave msg */
                sprintf(msg_send->username, "%s", username);
                sprintf(msg_send->room, "%s", chatroom);
                msg_send->type = LEAVE;
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
 
                /*Delete room data Structure */
                del_room(m_room);
                printf("Leaving %s. \n", chatroom);
            }
            sprintf(r_name, "%s", command + 2);
            m_room = room_init(r_name);
            sprintf(chatroom, "%s", command + 2);
            sprintf(server_room_group, "%sS%d", command + 2, proc_index);
            ret = SP_join(Mbox, server_room_group);
            sprintf(msg_send->username, "%s", username);
            sprintf(msg_send->room, "%s", chatroom);
            msg_send->type = JOIN;
            ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
 
            in_room = 1;
        /* Append message */
        case 'a':
            if(!in_room) {
                printf("Not in a Room\n");
                break;
            }
            sprintf(msg_send->username, "%s", username);
            sprintf(msg_send->room, "%s", chatroom);
            sprintf(msg_send->payload, "%s", command + 2);
            msg_send->type = MSG;
            ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
            break;
        /* Like a message */
        case 'l':
            if(!in_room) {
                printf("Not in a Room\n");
                break;
            }
            like_lts = get_lts(m_room, atoi(command + 2));
            if(like_lts.server != -1 && like_lts.index != -1) {
                memcpy(msg_send->payload, &like_lts, sizeof(lts));
                sprintf(msg_send->username, "%s", username);
                sprintf(msg_send->room, "%s", chatroom);
                msg_send->type = LIKE;
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);

            }
            else {
                printf("Not a valid line number.\n");
            }
            break;
        /* Remove a like */
        case 'r':
            if(!in_room) {
                printf("Not in a Room\n");
                break;
            }
            like_lts = get_lts(m_room, atoi(command + 2));
            if(like_lts.server != -1 && like_lts.index != -1) {
                memcpy(msg_send->payload, &like_lts, sizeof(lts));
                sprintf(msg_send->username, "%s", username);
                sprintf(msg_send->room, "%s", chatroom);
                msg_send->type = UNLIKE;
                ret = SP_multicast(Mbox, SAFE_MESS, server_group, 2, sizeof(serv_msg), (char *) msg_rec);
            }
            break;
        /* View full history */
        case 'h':
            if(!in_room) {
                printf("Not in a Room\n");
                break;
            }
            printf("\n");
            print_room(m_room, 0);
            break;
        /* View other servers */
        case 'v':
            /*TODO*/
            break;
    }
    printf(">");
}

void Read_message()
{
    static char in_mess[MAX_MESSLEN];
    char sender[MAX_GROUP_NAME];
    char target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
    int num_groups;
    int service_type = 0;
    int endian_mismatch;
    int16 mess_type;
    membership_info memb_info;
    change_mem c_m;

    /* Receive messages */
    ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups,
                      &mess_type, &endian_mismatch, sizeof(serv_msg), (char *) msg_rec );
    ret = SP_get_memb_info( in_mess, service_type, &memb_info );

    if( Is_regular_mess( service_type ) )
    {
        printf("Got a regular message\n");

        /* If about my room */
        if(in_room != 0 && !strcmp(target_groups[0], server_room_group))
        {
            /* Handle message */
            if(msg_rec->type == MSG) {
                if(room_insert_msg(m_room, msg_rec)) {
                    printf("\n");
                    print_room(m_room, 1);
                    printf("\n>");
                }
            }
            if(msg_rec->type == LIKE || msg_rec->type == UNLIKE) {
                c_m = room_insert_like(m_room, msg_rec);
                if(c_m.change){
                    printf("\n");
                    print_room(m_room, 1);
                    printf("\n>");
                }
                if(c_m.msg != NULL) {
                    free(c_m.msg);
                }
            }
            if(msg_rec->type == JOIN || msg_rec->type == LEAVE) {
                room_update_user(m_room, msg_rec);
                printf("\n");
                print_room(m_room, 1);
                printf("\n>");
            }
        }
    }
}

void checkError(char * action) {
    if (ret < 0)
    {
        printf("Error performing %s\n", action);
    }
}
