#include "analyze_text.h"
#include <ctype.h>
#include <string.h>

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

    return (*str == '\0') ? NULL : str;
}

int check_and_skip_comment_or_empty_line(char *line) {
    /* Checks if the line is a comment or empty */
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return 1;
    }
    return 0;
}


