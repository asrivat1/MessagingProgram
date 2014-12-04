#include "lamp_struct.h"


lamp_struct * lamp_struct_init();
void lamp_struct_insert(lamp_struct * ls, serv_msg * msg);
int * lamp_array(lamp_struct * ls);

lamp_struct * lamp_struct_init(){
    int i;
    lamp_struct * ls = malloc(sizeof(lamp_struct));
    for(i = 0; i < 5; i++)
        ls->s_list[i] = stack_init();
    return ls;
}

void lamp_struct_insert(lamp_struct * ls, serv_msg * msg){
    stack_push(ls->s_list[msg->stamp.server], msg);
}

int * lamp_array(lamp_struct * ls){
    int * arr = malloc(sizeof(int) * 5);
    int i;
    for(i = 0; i < 5; i++) {
        arr[i] = stack_look(ls->s_list[i]);
    }
    return arr;
}
