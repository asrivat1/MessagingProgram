#include "room.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(){
    /* Test Default */
    printf("Testing Default\n");
    char room_name[] = "Generic Room";
    char trev_name[] = "Trevor";
    room * r = room_init(room_name);
    assert(r->size == 0);
    /* Test Insert Msg 
     * Do out of order 
     * */
    printf("Testing Insert Msg\n");
    serv_msg * msg = malloc(sizeof(serv_msg));
    msg->type = MSG;
    msg->stamp.server = 0;
    msg->stamp.index = 5;
    msg->payload[0] = 'p';
    msg->payload[1] = '2';
    msg->payload[2] = '\0';
    strcpy(msg->username, trev_name);
    room_insert_msg(r, msg);

    msg = malloc(sizeof(serv_msg));
    msg->type = MSG;
    msg->stamp.server = 0;
    msg->stamp.index = 1;
    msg->payload[0] = 'p';
    msg->payload[1] = '1';
    msg->payload[2] = '\0';
    strcpy(msg->username, trev_name);
    room_insert_msg(r, msg);

    msg = malloc(sizeof(serv_msg));
    msg->type = MSG;
    msg->stamp.server = 0;
    msg->stamp.index = 10;
    msg->payload[0] = 'p';
    msg->payload[1] = '3';
    msg->payload[2] = '\0';
    strcpy(msg->username, trev_name);
    room_insert_msg(r, msg);

    assert(r->t_head->next->msg->stamp.index == 1);
    assert(r->t_head->next->next->msg->stamp.index == 5);
    assert(r->t_head->next->next->next->msg->stamp.index == 10);

    /* Test Regular Like */
    printf("Testing Regular Like\n");
    msg = malloc(sizeof(serv_msg));
    msg->type = LIKE;
    msg->stamp.server = 3;
    msg->stamp.index = 2;
    lts lamp;
    lamp.server = 0;
    lamp.index = 1;
    char * c = (char *) &lamp;
    int i;
    for(i = 0; i < sizeof(lts); i++)
        msg->payload[i] = c[i];
    strcpy(msg->username, trev_name);
    room_insert_like(r, msg);
    assert(r->t_head->next->likes->num_likes == 1);

    /* Test Dummy Like */
    printf("Testing Dummy Like\n");
    msg = malloc(sizeof(serv_msg));
    msg->type = LIKE;
    msg->stamp.server = 4;
    msg->stamp.index = 2;
    lamp.server = 0;
    lamp.index = 3;
    c = (char *) &lamp;
    for(i = 0; i < sizeof(lts); i++)
        msg->payload[i] = c[i];
    strcpy(msg->username, trev_name);
    room_insert_like(r, msg);
    assert(r->t_head->next->next->msg->type == DUMMY);

    /* Test Insert Msg over dummy like */
    printf("Testing Insert Msg over Dummy\n");
    msg = malloc(sizeof(serv_msg));
    msg->type = MSG;
    msg->stamp.server = 0;
    msg->stamp.index = 3;
    msg->payload[0] = 'p';
    msg->payload[1] = '1';
    msg->payload[2] = '.';
    msg->payload[3] = '5';
    msg->payload[4] = '\0';
    strcpy(msg->username, trev_name);
    room_insert_msg(r, msg);
    assert(r->t_head->next->next->msg->type == MSG);

    /* Test print room
     * Not really a real test, just a print!*/
    printf("Testing Print\n");
    print_room(r, 0);
    printf("Testing del\n");
    del_room(r);
    return 1;
}
