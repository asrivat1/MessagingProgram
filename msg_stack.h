#ifndef MSG_STACK
#define MSG_STACK

#include "server_include.h"

typedef struct msg_stack{
    serv_msg ** arr;
    int size;
    int arr_size;
}msg_stack;
/*msg stack functions */

/* init a stack */
msg_stack * stack_init();

/* push message onto stack */
void stack_push(msg_stack * ms, serv_msg * msg);

/* look at top */
int stack_look(msg_stack * ms); 

#endif /* MSG_STACK */
