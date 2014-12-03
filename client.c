#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sp.h>
#include<sys/time.h>

#define MAX_MEMBERS  100
#define MAX_MESSLEN  102400
#define WINDOW 60

typedef struct pack{
    int rand;
    int proc;
    int index;
    char payload[1200];
}pack;

static mailbox Mbox;
static char Private_group[MAX_GROUP_NAME];
char group_name[] = "TA";
char start[] = "Start";
int ret;
int num_members;
int connected_members = 0;
int done_members = 0;
FILE *fd;
int num_packets;
int num_sent = 1;
int proc_index;
int sent_done = 0;
double time;
pack *p_buff;
pack *t_pack;
static struct timeval begin;
static struct timeval end;


void handle_input(int argc, char *argv[]);
void Read_message();
void checkError(char * action);

int main(int argc, char *argv[])
{
    p_buff = malloc(sizeof(pack));
    t_pack = malloc(sizeof(pack));
    handle_input(argc, argv);
    char spread_name[] = "10210";
    char User[] = "user";
    sp_time test_timeout;

    test_timeout.sec = 5;
    test_timeout.usec = 0;

    /* Connect and join */
	ret = SP_connect_timeout( spread_name, User, 0, 1, &Mbox, Private_group, test_timeout );
    checkError("Connect");
    ret = SP_join(Mbox, group_name);
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
    int num_groups;
    int service_type = 0;
    int endian_mismatch;
    int16 mess_type;
    membership_info memb_info;
    int i;

    /* Receive messages */
    ret = SP_receive( Mbox, &service_type, sender, MAX_MEMBERS, &num_groups, target_groups,
                      &mess_type, &endian_mismatch, sizeof(pack), (char *) p_buff );
    ret = SP_get_memb_info( in_mess, service_type, &memb_info );

    if( Is_regular_mess( service_type ) )
    {
        /* If it receives  msg, all members have connected */
        if(connected_members < num_members)
        {
            connected_members = num_members;
            gettimeofday(&begin, NULL);
            printf("All Members Connected \n");
            printf("Begining Sending \n");
            /* Send initial salvo */
            for(i = 0; i < WINDOW; i++) {
                /* Send finishing pack if done sending */
                if (num_sent > num_packets) {
                    t_pack->proc = proc_index; 
                    t_pack->index = -1;
                    sent_done = 1;
                    ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                        sizeof(pack), (char *) t_pack );
                    break;
                }
                t_pack->rand = rand() % 1000000;
                t_pack->index = num_sent++;
                t_pack->proc = proc_index; 
                ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                    sizeof(pack), (char *) t_pack );
            }
        }

        /*Is it a completion message? */
        if(p_buff->index == -1) {
            printf("Process %d done sending messages \n", p_buff->proc);
            if(++done_members == num_members) {
                gettimeofday(&end, NULL);
                time = (end.tv_sec + end.tv_usec / 1000000.0
                        - begin.tv_sec - begin.tv_usec /  1000000.0);
                printf("Time taken: %.4f \n", time);
                free(p_buff);
                free(t_pack);
                fclose(fd);
                exit(0);
            }
        }
        /* Otherwise it is a regular message*/
        else {
            /* Deliver packets */
            fprintf(fd, "%2d, %8d, %8d\n", p_buff->proc, p_buff->index, p_buff->rand);
            /* Is it from me? */
            if(p_buff->proc == proc_index) {
                /* Send finishing pack if done sending */
                if(num_sent > num_packets) {
                    if (!sent_done) {
                        t_pack->index = -1;
                        t_pack->proc = proc_index; 
                        ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                            sizeof(pack), (char *) t_pack );
                        sent_done = 1;
                    }
                }
                /* Otherwise send another pack */
                else {
                    t_pack->rand = rand() % 1000000;
                    t_pack->index = num_sent++;
                    t_pack->proc = proc_index; 
                    ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                    sizeof(pack), (char *) t_pack );
                }
            }
        }
    }
    else if( Is_caused_join_mess(service_type))
    {
        printf("New member joined: %s\n", memb_info.changed_member);
        connected_members++;

        /* If all members connected, send initial info */
        if(connected_members == num_members)
        {
            printf("All Members Connected \n");
            printf("Begining Sending \n");
            gettimeofday(&begin, NULL);
            /* Send initial salvo */
            for(i = 0; i < WINDOW; i++) {
                /* Send finishing pack if done sending */
                if (num_sent > num_packets) {
                    t_pack->index = -1;
                    ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                        sizeof(pack), (char *) t_pack );
                    break;
                }
                t_pack->rand = rand() % 1000000;
                t_pack->index = num_sent++;
                t_pack->proc = proc_index; 
                ret = SP_multicast( Mbox, AGREED_MESS, group_name, 2,
                                    sizeof(pack), (char *) t_pack );
            }
        }
    }
}

void handle_input(int argc, char *argv[]) {
    char file_name[8];
    if(argc < 3) {
        perror("Mcast:Argument Error");
        exit(1);
    }
    
    sscanf(argv[1], "%d", &num_packets);
    sscanf(argv[2], "%d", &proc_index);
    sscanf(argv[3], "%d", &num_members);
    /*Validate Input */
    if(num_members > 10 || num_members < 0 || proc_index > num_members ||
       proc_index < 0 || num_packets < 0) {
        perror("Mcast: Incorrect Arguments");
        exit(1);
    }
    /*Set up file */
    snprintf(file_name, sizeof(file_name), "%d.out", proc_index);
    fd = fopen(file_name, "w");
}

void checkError(char * action) {
    if (ret < 0)
    {
        printf("Error performing %s\n", action);
    }
}

