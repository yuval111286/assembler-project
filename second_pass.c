#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "utils.h"
#include "errors_handler.h"


char *two_binary_digits_one_base4_char (int binary_num){

    if (binary_num == 0) return "a";
    if (binary_num == 1) return "b";
    if (binary_num == 2) return "c";
    if (binary_num == 3) return "d";

    return NULL;
}

int binary_line_to_base_4(char *code){

    unsigned int temp;
    int i = 6,j;
    char *letter;


    code[i] = '\0';


    for (j = 0; j < 5; j++) {
        letter = two_binary_digits_one_base4_char(temp & 3);
        if (letter == NULL) {
            return -1;
        }
        i--;
        code[i] = letter[0];
        temp >>= 2;
    }

    return 0;
}

int create_obj_file(char *obj_file_name){

    FILE *fp_obj;
    char *code_line, *address_line;
    int i;

    fp_obj = fopen(obj_file_name,"w");
    if (fp_obj == NULL){
        return -1;
    }

    code_line = malloc_allocation(6);
    address_line = malloc_allocation(6);
    if (code_line == NULL) {
        return -1;
    }
    if (address_line == NULL) {
        return -1;
    }

    for (i = 0; i < 8; i++) {
        binary_line_to_base_4(address_line);
        binary_line_to_base_4(code_line);

        fputs(address_line, fp_obj);
        fputs("\t", fp_obj);
        fputs(code_line, fp_obj);
        fputs("\n", fp_obj);
    }

    fclose(fp_obj);
    return 0;
}

int create_ent_file(char *ext_file_name){
    FILE *fp_ext;
    char *code_line;
    int i;

    fp_ext = fopen(ext_file_name,"w");
    if (fp_ext == NULL){
        return -1;
    }

    code_line = malloc_allocation(6);
    if (code_line == NULL) {
        return -1;
    }


    for (i = 0; i < 8; i++) {
        binary_line_to_base_4(code_line);

        fputs("address_line", fp_ext);
        fputs("\t", fp_ext);
        fputs(code_line, fp_ext);
        fputs("\n", fp_ext);
    }

    fclose(fp_ext);
    return 0;
}

int create_ext_file(char *ext_file_name){
    FILE *fp_ent;
    char *code_line;
    int i;

    fp_ent = fopen(ext_file_name,"w");
    if (fp_ent == NULL){
        return -1;
    }

    code_line = malloc_allocation(6);
    if (code_line == NULL) {
        return -1;
    }


    for (i = 0; i < 8; i++) {
        binary_line_to_base_4(code_line);

        fputs("address_line", fp_ent);
        fputs("\t", fp_ent);
        fputs(code_line, fp_ent);
        fputs("\n", fp_ent);
    }

    fclose(fp_ent);
    return 0;
}



int create_files(char *org_file_name){

    /* create files*/

    char *obj_file_name,*ent_file_name,*ext_file_name;
    int success = 1;

    obj_file_name = change_ending_of_file(org_file_name, ".obj");

    if (obj_file_name == NULL){
        printf("could not set obj file name");
        return -1;
    }


    success = create_obj_file(obj_file_name);
     if(success)
     {
         error_log(org_file_name,-1,"Could not create obj file");
         return -1;
     }

     /*check if entry is not null*/
    success = 1;

    ent_file_name = change_ending_of_file(org_file_name, ".ent");
    success = create_ent_file(ent_file_name);
    if(success)
    {
        error_log(org_file_name,-1,"Could not create ent file");
        return -1;
    }


    /*check if extern is not null*/
    success = 1;
    ext_file_name = change_ending_of_file(org_file_name, ".ext");
    success = create_ext_file(ext_file_name);
    if(success)
    {
        error_log(org_file_name,-1,"Could not create ext file");
        return -1;
    }

    return 0;
}

int second_pass(char *file_name,SymbolTable *symbol_table, CodeImage *code_image){

    return 0;
}







