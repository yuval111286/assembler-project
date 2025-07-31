#include "second_pass.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char *two_binary_digits_one_base4_char (int binary_num){

    if (binary_num == 0) return "a";
    if (binary_num == 1) return "b";
    if (binary_num == 2) return "c";
    if (binary_num == 3) return "d";

    return NULL;
}

int binary_line_to_base_4(int binary_line, char *code){

    unsigned int temp;
    int i = 4,j;/*counter of bits of 1*/
    char *letter;


    code[i] = '\0';


    for (j = 0; j < 4; j++) {
        letter = two_binary_digits_one_base4_char(temp & 3);
        if (letter == NULL) {
            return -1;
        }
        code[i] = letter[0];
        i--;
        temp >>= 2;
    }

    return 0;
}


int second_pass()
{

}