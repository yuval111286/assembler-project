#include "analyze_text.h"


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


int check_and_skip_comment_or_empty_line(char *line) {
    /* Checks if the line is a comment or empty */
    if (*line == ';' || *line == '\0' || *line == '\n') {
        return 1;
    }
    return 0;
}