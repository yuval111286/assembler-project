#ifndef ASSEMBLER_PROJECT_ANALYZE_TEXT_H
#define ASSEMBLER_PROJECT_ANALYZE_TEXT_H

#include <stdio.h>   /* For FILE type in print_error */

/**
 * @param str line from file
 * @returns clean string without space in the start and the end of the string.
 */
char *trim_spaces(char *str);

/**
 *
 * @param line the tested line
 * @return 1 if the line is a comment or empty 0 otherwise
 */
int check_and_skip_comment_or_empty_line(char *line);

FILE* create_clean_file(char* input_file_name, char* output_file_name);

char *skip_spaces(char *str);

char *skip_word(char *str);

#endif /*ASSEMBLER_PROJECT_ANALYZE_TEXT_H*/