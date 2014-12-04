#include "msg_stack.h"

msg_stack * stack_init();
void stack_push(msg_stack * ms, serv_msg * msg);
int stack_look(msg_stack * ms);


msg_stack * stack_init(){
    msg_stack * ms = malloc(sizeof(msg_stack));
    serv_msg ** arr = malloc(sizeof(serv_msg *) * 10);
    ms->arr = arr;
    ms->size = 0;
    ms->arr_size = 10;
    return ms;
}


void stack_push(msg_stack *ms, serv_msg * msg){
    if(++(ms->size) > ms->arr_size){
        ms->arr_size *= 2;
        ms->arr = realloc(ms->arr, ms->arr_size * sizeof(serv_msg *) );
    }
    ms->arr[ms->size - 1] = msg; 
}


int stack_look(msg_stack * ms){
    return (ms->size != 0)?ms->arr[ms->size - 1]->stamp.index: -1;
}
