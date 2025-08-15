#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "preprocessor_helper.h"
#include "errors_handler.h"
#include "utils.h"



char *skip_one_word(char *word) {
    if (word == NULL) return NULL;

    /* Skip current word */
    while (*word != '\0' && !isspace(*word)) {
        word++;
    }

    /* Skip spaces after the word */
    while (isspace(*word)) {
        word++;
    }

    /* No more text after this word */
    if (*word == '\0') return NULL;

    return word;
}



char *copy_text_from_file_to_string(FILE *fp, fpos_t *pos, int len_of_chars_to_copy) {
    int i, current_char;
    char *str;

    /* Validate input parameters */
    if (fp == NULL || pos == NULL || len_of_chars_to_copy <= 0) {
        return NULL;
    }

    /* Restore file position to the saved position */
    if (fsetpos(fp, pos) != 0) {
        printf(FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }

    /* Allocate memory for the string  */
    str = malloc_allocation(len_of_chars_to_copy + 1);
    if (str == NULL) {
        return NULL;
    }

    /* Read up to len_of_chars_to_copy characters */
    for (i = 0; i < len_of_chars_to_copy; i++) {
        current_char = getc(fp);

        if (current_char == EOF) {
            break;
        }

        str[i] = (char)current_char;
    }

    str[i] = '\0';

    /* Save the current file position after reading */
    fgetpos(fp, pos);

    return str;
}



int check_line_comment_or_empty(char *line) {
    /* Returns 1 if the line is a comment or empty */
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return 1;
    }
    return 0;
}

FILE* create_clean_file(char* input_file_name, char* output_file_name) {
    FILE *input_file, *output_file;
    char line[MAX_LINE_LENGTH], line_copy[MAX_LINE_LENGTH], *clean;
    int len;

    /* Open original file for reading */
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        error_log(input_file_name, 0, FILE_NOT_OPEN_READING);
        return NULL;
    }

    /* Open output file for writing */
    output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        error_log(output_file_name, 0, FILE_NOT_OPEN_WRITING);
        fclose(input_file);
        return NULL;
    }

    while (fgets(line, sizeof(line), input_file) != NULL) {
        clean = cut_spaces_before_and_after_string(line);

        /* Only process lines that are not comments or empty */
        if (!check_line_comment_or_empty(clean)) {
            strcpy(line_copy, line); /* Copy before trimming again */
            clean = cut_spaces_before_and_after_string(line_copy);

            /* Write cleaned line to the output file */
            fputs(clean, output_file);

            /* Ensure each line ends with a newline */
            len = strlen(clean);
            if (len > 0 && clean[len-1] != '\n') {
                fputs("\n", output_file);
            }
        }
    }


    fclose(input_file);
    fclose(output_file);

    return output_file;
}