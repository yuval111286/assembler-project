#include "analyze_text.h"
#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "errors_handler.h"

char *cut_spaces_before_and_after_string(char *str) {
    char *end_pointer_of_string;
    /* skip leading whitespace */
    while (isspace(*str)) str++;

    /*empty string*/
    if (*str == '\0') return str;

    /* find end of string and cut trailing whitespace */
    end_pointer_of_string = str + strlen(str) - 1;
    while (end_pointer_of_string > str && isspace(*end_pointer_of_string)) {
        /*short the end of string from empty chars*/
        *end_pointer_of_string = '\0';
        end_pointer_of_string--;
    }
    return str;
}

char *skip_one_word(char *word) {

    if (word == NULL) return NULL;

    /* skip current word */
    while (*word != '\0' && !isspace(*word)) {
        word++;
    }

    /* skip spaces after the word */
    while (isspace(*word)) {
        word++;
    }

    /*no more text after this word*/
    if (*word == '\0') return NULL;

    return word;
}

int check_line_comment_or_empty(char *line) {
    /* checks if the line is a comment or empty */
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return 1;
    }
    return 0;
}


FILE* create_clean_file(char* input_file_name, char* output_file_name) {
    FILE *input_file, *output_file;
    char line[MAX_LINE_LENGTH], line_copy[MAX_LINE_LENGTH], *clean;
    int len;

    /* open original file to read from*/
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        error_log(input_file_name,0,FILE_NOT_OPEN_READING);
        return NULL;
    }

    /* open output file to write to*/
    output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        error_log(output_file_name,0,FILE_NOT_OPEN_WRITING);
        fclose(input_file);
        return NULL;
    }

    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (!check_line_comment_or_empty(line)) {
            /* create copy string for trimming space */
            strcpy(line_copy, line);
            clean = cut_spaces_before_and_after_string(line_copy);

            /* write clean line to output file */
            fputs(clean, output_file);

            /* verify line ends with \n */
            len = strlen(clean);
            if (len > 0 && clean[len-1] != '\n') {
                fputs("\n", output_file);
            }
        }
    }

    /* Close both files */
    fclose(input_file);
    fclose(output_file);

    return output_file;
}


char *copy_text_from_file_to_string(FILE *fp, fpos_t *pos, int len_of_chars_to_copy) {
    int i,current_char;
    char *str;

    /* check input parameters are not empty*/
    if (fp == NULL || pos == NULL || len_of_chars_to_copy <= 0) {
        return NULL;
    }

    /* set the file position to the given saved position*/
    if (fsetpos(fp, pos) != 0) {
        printf(FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }

    /* allocate memory for the string (+1 for the null terminator)*/
    str = malloc_allocation(len_of_chars_to_copy + 1);
    if (str == NULL) {
        return NULL;
    }

    /* read up to 'len_of_chars_to_copy' characters from the file*/
    for (i = 0; i < len_of_chars_to_copy; i++) {
        current_char = getc(fp);

        /* stop if end-of-file is reached early*/
        if (current_char == EOF) {
            break;
        }

        /*placing current char in the string*/
        str[i] = (char)current_char;
    }

    /* putting null-terminate in the end of the string*/
    str[i] = '\0';

    /* save the current file position after reading*/
    fgetpos(fp, pos);

    return str;
}
