#include "lamp_struct.h"

lamp_struct * lamp_struct_init();
void lamp_struct_insert(lamp_struct * ls, serv_msg * msg);
serv_msg lamp_struct_pop(lamp_struct * ls);
int * lamp_array(lamp_struct * ls);

lamp_struct * lamp_struct_init(){
    int i;
    lamp_struct * ls = malloc(sizeof(lamp_struct));
    for(i = 0; i < 5; i++)
        ls->s_list[i] = list_init();
    return ls;
}

void lamp_struct_insert(lamp_struct * ls, serv_msg * msg){
    list_insert(ls->s_list[msg->stamp.server], msg);
}

serv_msg lamp_struct_pop(lamp_struct * ls){
    int i;
    int lowest = list_lowest(ls->s_list[0]);
    int lowest_server = 0;
    for(i = 1; i < 5; i++) {
        if(list_lowest(ls->s_list[i])!= -1 && list_lowest(ls->s_list[i])  < lowest) {
            lowest = list_lowest(ls->s_list[i]);
            lowest_server = i;
        }
    }
    return list_pop(ls->s_list[lowest_server]);
}

int * lamp_array(lamp_struct * ls){
    int * arr = malloc(sizeof(int) * 5);
    int i;
    for(i = 0; i < 5; i++) {
        arr[i] = list_highest(ls->s_list[i]);
    }
    return arr;
}
