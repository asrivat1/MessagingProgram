#include "lamp_struct.h"
#include <assert.h>
#include <stdio.h>

int main() {
    lamp_struct * ls = lamp_struct_init();
    serv_msg * ms1, * ms2, * ms3, * ms4;
    ms1 = malloc(sizeof(serv_msg));
    ms2 = malloc(sizeof(serv_msg));
    ms3 = malloc(sizeof(serv_msg));
    ms4 = malloc(sizeof(serv_msg));

    ms1->stamp.server = 1;
    ms1->stamp.index = 2;

    ms2->stamp.server = 1;
    ms2->stamp.index = 3;

    ms3->stamp.server = 4;
    ms3->stamp.index = 5;

    ms4->stamp.server = 2;
    ms4->stamp.index = 1;

    int * arr = lamp_array(ls);
    printf("TESTING INITIAL \n");
    int i;
    for(i = 0; i < 5; i++) {
        assert(arr[i] == -1);
    }
    free(arr);

    lamp_struct_insert(ls, ms1);
    lamp_struct_insert(ls, ms2);
    lamp_struct_insert(ls, ms3);
    lamp_struct_insert(ls, ms4);
    arr = lamp_array(ls);
    printf("TESTING AFTER ADDING \n");
    assert(arr[0] == -1);
    assert(arr[1] == 3);
    assert(arr[2] == 1);
    assert(arr[3] == -1);
    assert(arr[4] == 5);
    free(arr);

    printf("TESTING PRESERVATION \n");
    assert(ls->s_list[1]->arr[0]->stamp.index = 2);
    printf("TESTING DOUBLING \n");
    for(i = 0; i < 20; i++) {
        ms1 = malloc(sizeof(serv_msg));
        ms1->stamp.server = 1;
        ms1->stamp.index = 4 + i;
        lamp_struct_insert(ls, ms1);
    }
    ms1 = malloc(sizeof(serv_msg));
    ms1->stamp.server = 1;
    ms1->stamp.index = 45;
    lamp_struct_insert(ls, ms1);
    arr = lamp_array(ls);
    assert(arr[1] == 45);
    printf("ALL TESTS PASSED \n");
    return 1;
}
