#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "preprocessor.h"
#include "errors_handler.h"
#include "linked_list.h"
#include "analyze_text.h"
#include "utils.h"
#include "globals.h"
struct node *macro_list = NULL; /* Linked list of macros */


int check_as_file_ending(char *file_name)
{
    char *c;

    /* cuts the exising ending of a file  */
    c = strrchr(file_name, '.');
    if (strcmp(c, ".as") == 0){
        return 0;
    }
    return -1;
}

/*
 * - Creates clean version of source file
 * - Performs macro expansion
 * Returns 0 on success, -1 on failure
 */
int preprocessor_full_flow(char *file_name) {
    FILE *clean_file;                          /* Pointer to cleaned file */
    char clean_file_name[256];                 /* Name for output file */

    /* Check that the input file ends with .as */
    if (check_as_file_ending(file_name) != 0) {
        printf("Error: Did not receive .as file\n");
        return -1; /* Invalid file extension */
    }

    /* Generate new cleaned file name, e.g., prog.as_clean.as */
    snprintf(clean_file_name, sizeof(clean_file_name), "%s_clean.as", file_name);

    /* Create cleaned version of the file (removes whitespace/comments) */
    clean_file = create_clean_file(file_name, clean_file_name);
    if (clean_file == NULL) {
        printf("Error: Failed to create clean file\n");
        return -1; /* Failed to clean */
    }

    /* Perform macro extraction and expansion */
    prepro_first_pass();     /* First pass: store macro definitions */
    preproc_second_pass();   /* Second pass: expand macros */

    return 0; /* Success */
}


/*
 * prepro_first_pass
 * First pass of preprocessing:
 * - Opens the cleaned file
 * - Scans each line
 * - If "mcro" is found, extracts and stores macro name and body
 * - Adds each macro as a node to a linked list
 */
void prepro_first_pass() {
    FILE *file;                                   /* File pointer to cleaned file */
    char line[MAX_LINE_LENGTH];                   /* Current line buffer */
    char *macro_name = NULL;                      /* Name of the macro */
    char *macro_text = NULL;                      /* Text content (body) of macro */
    int inside_macro = 0;                         /* Flag indicating we're inside macro definition */
    int line_num = 0;                             /* Line counter for error reporting */
    char *name_token;                             /* Token for extracting macro name */

    /* Open the cleaned file for reading */
    file = fopen("prog.as_clean.as", "r");
    if (file == NULL) {
        internal_error_log(FILE_NOT_OPEN_READING); /* Report error if file can't be opened */
        return;
    }

    /* Loop over each line in the file */
    while (fgets(line, sizeof(line), file) != NULL) {
        line_num++; /* Increment line counter */

        /* Check if line starts with "mcro" */
        if (strstr(line, "mcro") == line) {
            name_token = strtok(line + 4, " \t\n"); /* Skip "mcro" and get name */

            if (name_token == NULL) {
                internal_error_log(MACRO_WITHOUT_NAME); /* No name after "mcro" */
            } else {
                /* Allocate memory for macro name */
                macro_name = (char *) malloc(strlen(name_token) + 1);
                if (macro_name == NULL) {
                    internal_error_log(MEMORY_FAIL);
                    fclose(file);
                    return;
                }
                strcpy(macro_name, name_token); /* Copy macro name */

                /* Allocate memory for macro body */
                macro_text = (char *) malloc_allocation(MAX_LINE_LENGTH * MAX_LINE_LENGTH);
                if (macro_text == NULL) {
                    internal_error_log(MEMORY_FAIL);
                    free(macro_name);
                    fclose(file);
                    return;
                }
                macro_text[0] = '\0'; /* Initialize empty body */
                inside_macro = 1;     /* Start recording macro body */
            }
        }
        /* Check if line is "endmcro" */
        else if (strstr(line, "endmcro") == line) {
            if (!inside_macro) {
                internal_error_log(MACROEND_WITHOUT_START); /* "endmcro" without "mcro" */
            } else {
                add_node_to_linked_list(&macro_list, macro_name, macro_text, line_num); /* Save macro */
            }

            /* Reset state */
            inside_macro = 0;
            macro_name = NULL;
            macro_text = NULL;
        }
        /* Any line inside a macro body */
        else {
            if (inside_macro && macro_text != NULL) {
                strcat(macro_text, line); /* Append line to macro body */
            }
        }
    }

    /* Close the file when done */
    fclose(file);
}

   /* To be writting preproc_second_pass();*/

