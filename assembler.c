#include <stdio.h>
#include <stdlib.h>
#include "first_pass.h"
#include "second_pass.h"
#include "preprocessor.h"
#include "user_interface.h"

/**
 *@brief MAIN function of assembler program
 * Contains three main stages:
 * 1. PREPROCESSOR - expands macros and creates .am file
 * 2. First pass - performs initial encoding and builds symbol table
 * 3. Second pass - completes encoding and generates output files
 *
 * @param argc num of as files to process
 * @param argv array of as name files
 * @return 0 for end of the assembler program
 *
 * @note The function handles multiple input files sequentially, providing user feedback for each processing stage
 * and error reporting.
 * @note If no errors are found output files (.am, .ob, .ent, .ext) are generated
 */
int main(int argc, char *argv[])
{
    char *as_file=NULL, *am_file=NULL;
    node *mcro_head;
    SymbolTable symbol_table;
    CodeImage code_image;
    unsigned int data_image[MAX_DATA_SIZE];
    int IC_final, DC_final, file_name_len, discover_errors = 0;

    /*printing welcome message */
    printf(WELCOME);
    while (--argc > 0) {

        file_name_len = strlen(argv[argc]);

        /*checking for long file name*/
        if (file_name_len > MAX_FILE_NAME_LENGTH){
            printf(LONG_FILE_NAME, argv[argc]);
            continue;
        }

        as_file = argv[argc];
        mcro_head = NULL;
        printf(PREPROC);
        /*performing preprocessor step*/
        if (preprocessor_full_flow(as_file, &mcro_head,&am_file)) {
            if (mcro_head) {
                free_linked_list(mcro_head);
            }
            printf(FINISH, as_file);
            continue;
        }
        printf(PREPROC_END);


        init_symbol_table(&symbol_table);
        IC_final = 0;
        DC_final = 0;

        /*performing first pass step*/
        printf(FIRST_PASS);
        discover_errors = first_pass(am_file, &symbol_table, &IC_final, &DC_final, &code_image, &mcro_head,data_image);

        if (!discover_errors){
            printf(FIRST_PASS_END);
        }
            /*free mcro linked list after use*/
        if (mcro_head) {
            free_linked_list(mcro_head);
        }

        /*performing second pass*/
        printf(SECOND_PASS);
        discover_errors = second_pass(am_file, &symbol_table, &code_image, IC_final, DC_final, data_image,discover_errors);
        if (!discover_errors){
            printf(SECOND_PASS_END);
        }

        /*releasing all resources*/
        free_symbol_table(&symbol_table);
        printf(FINISH, as_file);
    }

    /*finish going over all files*/
    printf(GOODBYE);
    return 0;
}