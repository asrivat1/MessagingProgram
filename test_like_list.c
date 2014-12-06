#include "like_list.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main() {
    like_list * ll = like_list_init();
    change_mem c_m;
    serv_msg * msg;
    char n1[] = "Trevor";
    char n2[] = "Joe";
    char n3[] = "Bob";
    /*Test Default */
    printf("Testing Default \n");
    assert(ll->num_likes == 0);

    /*Test First Unlike */
    printf("Testing First Unlike \n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 1;
    msg->type = UNLIKE;
    strcpy(msg->username, n1);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 0);
    assert(c_m.msg == NULL);
    assert(ll->num_likes == 0);

    /*Test First Like */
    printf("Testing First Like \n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 1;
    msg->type = LIKE;
    strcpy(msg->username, n2);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 1);
    assert(c_m.msg == NULL);
    assert(ll->num_likes == 1);

    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 1;
    msg->type = LIKE;
    strcpy(msg->username, n3);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 1);
    assert(c_m.msg == NULL);
    assert(ll->num_likes == 2);

    /*Test Unliking an Unlike */
    printf("Testing Unliking an Unlike\n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 0;
    msg->stamp.index = 1;
    msg->type = UNLIKE;
    strcpy(msg->username, n1);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 0);
    assert(c_m.msg);
    free(c_m.msg);
    assert(ll->num_likes == 2);

    /*Test Liking a Like */
    printf("Testing Liking a Like\n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 3;
    msg->type = LIKE;
    strcpy(msg->username, n2);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 0);
    assert(c_m.msg);
    free(c_m.msg);
    assert(ll->num_likes == 2);

    /*Test Liking an Unlike */
    printf("Testing Liking an Unlike\n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 0;
    msg->stamp.index = 5;
    msg->type = LIKE;
    strcpy(msg->username, n1);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 1);
    assert(c_m.msg);
    free(c_m.msg);
    assert(ll->num_likes == 3);

    /*Test Unliking a Like */
    printf("Testing Unliking a Like\n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 5;
    msg->type = UNLIKE;
    strcpy(msg->username, n2);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 1);
    assert(c_m.msg);
    free(c_m.msg);
    assert(ll->num_likes == 2);

    /*Test Old LTS */
    printf("Testing Inserting an Old LTS\n");
    msg = malloc(sizeof(serv_msg));
    msg->stamp.server = 2;
    msg->stamp.index = 2;
    msg->type = LIKE;
    strcpy(msg->username, n2);
    c_m = like_list_update(ll, msg);
    assert(c_m.change == 0);
    assert(c_m.msg);
    free(c_m.msg);
    assert(ll->num_likes == 2);
    del_like_list(ll);

    printf("All Tests Passed\n");

    return 1;
}
