#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "preprocessor.h"
#include "errors_handler.h"

int main(int argc, char *argv[])
{
    char *as_file, *am_file;
    node *mcro_head;
    SymbolTable symbol_table;
    CodeImage code_image;
    unsigned int data_image[MAX_DATA_SIZE];
    int IC_final, DC_final, file_name_len;

    /*printing welcome message */
    printf(WELCOME);
    while (--argc > 0) {

        file_name_len = strlen(argv[argc]);

        /*checking for long file name*/
        if (file_name_len > MAX_FILE_NAME_LENGTH){
            printf(LONG_FILE_NAME, argv[argc]);
            continue;
        }

        as_file = change_ending_of_file(argv[argc], ".as");

        mcro_head = NULL;
        printf(PREPROC);
        /*performing preprocessor step*/
        if (preprocessor_full_flow(as_file, &mcro_head)) {
            if (mcro_head) {
                free_linked_list(mcro_head);
            }
            free(as_file);
            printf(FINISH, argv[argc]);
            continue;
        }

        init_symbol_table(&symbol_table);
        IC_final = 0;
        DC_final = 0;
        am_file = change_ending_of_file(argv[argc], ".am");

        /*performing first pass step*/
        printf(FIRST_PASS);
        first_pass(am_file, &symbol_table, &IC_final, &DC_final, &code_image, &mcro_head,data_image);

        /*free mcro linked list after use*/
        if (mcro_head) {
            free_linked_list(mcro_head);
        }

        /*performing second pass*/
        printf(SECOND_PASS);
        second_pass(am_file, &symbol_table, &code_image, IC_final, DC_final, data_image);

        /*releasing all resources*/
        free_symbol_table(&symbol_table);
        free(as_file);
        free(am_file);
        printf(FINISH, argv[argc]);
    }

    /*finish going over all files*/
    printf(GOODBYE);
    return 0;
}