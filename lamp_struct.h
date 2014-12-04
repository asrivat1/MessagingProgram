#ifndef LAMP_STRUCT
#define LAMP_STRUCT

#include "server_include.h"
#include "msg_stack.h"
/*
 * Lamport time stamp structure
 * Essentially an array of linked lists with some added functionality
 */
typedef struct lamp_struct{
    msg_stack * s_list[5];
}lamp_struct;


/*Create new lamp struct */
lamp_struct * lamp_struct_init();
/*Insert to lamp_struct*/
void lamp_struct_insert(lamp_struct * ls, serv_msg * msg);
/*return array of highest lts*/
int * lamp_array(lamp_struct * ls);

#endif /* LAMP_STRUCT */
