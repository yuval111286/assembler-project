#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "preprocessor.h"

int main(int argc, char *argv[])
{
    char *as_file, *am_file;
    node *mcro_head;
    SymbolTable symbol_table;
    CodeImage code_image;
    unsigned int data_image[MAX_DATA_SIZE];
    int IC_final, DC_final, file_name_len;

    while (--argc > 0) {

        file_name_len = strlen(argv[argc]);

        if (file_name_len > MAX_FILE_NAME_LENGTH)
        {
            printf("File name too long, moving to next file %s \n\n", argv[argc]);
            continue;
        }

        as_file = change_ending_of_file(argv[argc], ".as");

        mcro_head = NULL;
        printf("-- PREPROCESSOR --\n");
        if (preprocessor_full_flow(as_file, &mcro_head)) {
            if (mcro_head) {
                free_linked_list(mcro_head);
            }
            free(as_file);
            printf("Finish processing file %s \n\n", argv[argc]);
            continue;
        }

        init_symbol_table(&symbol_table);
        IC_final = 0;
        DC_final = 0;
        am_file = change_ending_of_file(argv[argc], ".am");

        printf("-- FIRST PASS --\n");
        if (first_pass(am_file, &symbol_table, &IC_final, &DC_final, &code_image, &mcro_head,data_image)) {
            free_symbol_table(&symbol_table);
            if (mcro_head) {
                free_linked_list(mcro_head);
            }
            free(as_file);
            free(am_file);
            printf("Finish processing file %s \n\n", argv[argc]);
            continue;
        }

        /*free mcro linked list after use*/
        if (mcro_head) {
            free_linked_list(mcro_head);
        }

        printf("-- SECOND PASS --\n");
        if (second_pass(am_file, &symbol_table, &code_image, IC_final, DC_final, data_image)) {
            free_symbol_table(&symbol_table);
            free(as_file);
            free(am_file);
            printf("Finish processing file %s \n\n", argv[argc]);
            continue;
        }

        free_symbol_table(&symbol_table);
        free(as_file);
        free(am_file);
        printf("Finish processing file %s \n\n", argv[argc]);
    }

    printf("\nNo more files, end of assembler program. \nHope you enjoyed!\n");
    return 0;
}