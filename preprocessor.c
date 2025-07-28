#include "preprocessor.h"

int preprocessor_full_flow(char file_name[]){

    FILE *fp_read, *fp_write, *first_copy;
    node *head = NULL;
    char line[MAX_LINE_LENGTH], *new_file_name;
    SourceFileLocation as_file;

    first_copy = create_clean_file(file_name,first_copy);

}