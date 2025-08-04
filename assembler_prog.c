#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "errors_handler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "preprocessor.h"
#include "code_image.h" 

int main(int argc, char *argv[])
{
    char *as_file, *am_file;
    node *head;
    SymbolTable symbol_table;
    CodeImage code_image;
    int IC_final, DC_final;

    while (--argc > 0) {
        as_file = change_ending_of_file(argv[argc], ".as");

        head = NULL;
        printf("-- PREPROCESSOR --\n");
        if (preprocessor_full_flow(as_file,&head)) {
            if (head) {
                free_linked_list(head);
            }
            free(as_file);
            return 1;
        }

        init_symbol_table(&symbol_table);
        IC_final = 0;
        DC_final = 0;
        am_file = change_ending_of_file(argv[argc], ".am");

        printf("-- FIRST PASS --\n");
        if (first_pass(am_file, &symbol_table, &IC_final, &DC_final, &code_image,&head)) {
            free_symbol_table(&symbol_table);
            if (head) {
                free_linked_list(head);
            }
            free(as_file);
            free(am_file);
            return 1;
        }

        printf("-- SECOND PASS --\n");
        if (second_pass(as_file)) {
            return 1;
        }
    }

    free_symbol_table(&symbol_table);
    if (head) {
        free_linked_list(head);
    }
    free(as_file);
    free(am_file);

    return 0;
}
