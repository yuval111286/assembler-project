#include "analyze_text.h"
#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "errors_handler.h"

char *trim_spaces(char *str) {
    char *end;
    /* Skip leading whitespace */
    while (isspace(*str)) str++;

    if (*str == '\0') return str;

    /* Find end of string and trim trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end = '\0';
        end--;
    }

    return str;
}

char *skip_spaces(char *str){

    /* Skip leading whitespace */
    while (isspace(*str)) str++;

    if (*str == '\0') return NULL;

    return str;
}

char *skip_word(char *str) {
    /* skip current word */
    while (*str != '\0' && !isspace(*str)) {
        str++;
    }

    /* skip spaces after the word */
    while (isspace(*str)) {
        str++;
    }

    if (*str == '\0') return NULL;

    return str;
}

int check_and_skip_comment_or_empty_line(char *line) {
    /* Checks if the line is a comment or empty */
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return 1;
    }
    return 0;
}



FILE* create_clean_file(char* input_file_name, char* output_file_name) {
    FILE *input_file, *output_file;
    char line[MAX_LINE_LENGTH], line_copy[MAX_LINE_LENGTH], *trimmed;
    int len;

    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
        error_log(input_file_name,0,FILE_NOT_OPEN_READING);
        return NULL;
    }

    output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        error_log(output_file_name,0,FILE_NOT_OPEN_WRITING);
        fclose(input_file);
        return NULL;
    }

    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (!check_and_skip_comment_or_empty_line(line)) {
            /* Make a copy for trimming (since trim_spaces modifies the string) */
            strcpy(line_copy, line);
            trimmed = trim_spaces(line_copy);

            /* Write trimmed line to output file */
            fputs(trimmed, output_file);

            /* Ensure line ends with \n */
            len = strlen(trimmed);
            if (len > 0 && trimmed[len-1] != '\n') {
                fputs("\n", output_file);
            }
        }
    }

    /* Close both files */
    fclose(input_file);
    fclose(output_file);

    return output_file;
}


char *copy_text_from_file_to_string(FILE *fp, fpos_t *pos, int len) {
    int i;
    char *str;

    if (fsetpos(fp, pos) != 0) {
        printf(FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }
    str = malloc_allocation((len + 1));
    if (str == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        str[i] = getc(fp);
    }
    str[len] = '\0';
    fgetpos(fp, pos);
    return str;
}