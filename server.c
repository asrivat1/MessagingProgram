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
char server_group[] = "Servers";
char User[] = "Server#";
char spread_name[] = "10210";
int ret;
FILE *fd;
int num_sent = 1;
int proc_index;
int sent_done = 0;
double time;
serv_msg * msg_send;
serv_msg * msg_rec;
static struct timeval begin;
static struct timeval end;

int group_status[NUM_SERVERS];
int prev_group_status[NUM_SERVERS];

void handle_input(int argc, char *argv[]);
void Read_message();
void handle_message(serv_msg * msg);
void merge();
void checkError(char * action);

int main(int argc, char *argv[])
{
    /* Allocate memory and handle input */
    msg_send = malloc(sizeof(serv_msg));
    msg_rec = malloc(sizeof(serv_msg));

    messages = lamp_struct_init();
    
    handle_input(argc, argv);

    lamport_time = malloc(sizeof(lts));
    lamport_time->server = proc_index;
    lamport_time->index = 1;

    sp_time test_timeout;
    test_timeout.sec = 5;
    test_timeout.usec = 0;

    /* Connect and join various groups */
	ret = SP_connect_timeout( spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
    checkError("Connect");
    ret = SP_join(Mbox, server_group);
    checkError("Join");

    /* Set up E - DO NOT MOVE */
	E_init();
    E_attach_fd( Mbox, READ_FD, Read_message, 0, NULL, HIGH_PRIORITY );
	E_handle_events();

    return 0;
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
    int i;

    /* Receive messages */
    ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups,
                      &mess_type, &endian_mismatch, sizeof(serv_msg), (char *) msg_rec );
    ret = SP_get_memb_info( in_mess, service_type, &memb_info );

    if( Is_regular_mess( service_type ) )
    {
        printf("Got a regular message\n");

        /* Ignore if from self */
        if(strcmp(sender, User) == 0)
        {
            return;
        }

        /* Increment LTS */
        if( !ltscomp(msg_rec->stamp, *lamport_time) )
        {
            lamport_time->index = 1 + msg_rec->stamp.index;
        }
        else
        {
            lamport_time->index = 1 + lamport_time->index;
        }

        /* Write to file */
        fprintf(fd, "%s\n", (char *) msg_rec);

        /* Add to list of messages and handle */
        lamp_struct_insert(messages, msg_rec);

        /* Send message to client */
        sprintf(client_group, "%s-%s", msg_rec->room, User);
        ret = SP_multicast(Mbox, SAFE_MESS, client_group, 2, sizeof(serv_msg), (char *) msg_rec);
    }
    else if( Is_reg_memb_mess(service_type))
    {
        /* If message from server_group */
        if (!strcmp(sender, server_group))
        {
            int server_index;
            int merge_case = 0;
            printf("Server group membership change.\n");
            for (i=0; i < NUM_SERVERS; i++)
            {
                prev_group_status[i] = group_status[i];
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
            if (merge_case)
            {
                /* Deal with it */
                printf("Merging!\n");
            }
        }
    }
}

void merge()
{
}

void handle_input(int argc, char * argv[]) {
    if(argc < 1) {
        perror("Mcast: Argument Error");
        exit(1);
    }
    
    /* Get server ID */
    sscanf(argv[1], "%d", &proc_index);
    User[7] = proc_index;

    /*Set up file */
    snprintf(User, sizeof(User), "%d.out", proc_index);
    fd = fopen(User, "w");
}

void checkError(char * action) {
    if (ret < 0)
    {
        printf("Error performing %s\n", action);
    }
}

