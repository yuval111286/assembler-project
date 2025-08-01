#ifndef ASSEMBLER_PROJECT_ANALYZE_TEXT_H
#define ASSEMBLER_PROJECT_ANALYZE_TEXT_H

#include <stdio.h>

/**
 * @param str line from file
 * @returns clean string without space in the start and the end of the string.
 */
char *trim_spaces(char *str);

/**
 *Checks if the line is a comment or empty
 *
 * @param line the tested line
 * @return 1 if the line is a comment or empty 0 otherwise
 */
int check_line_comment_or_empty(char *line);

/**
 *
 * @param input_file_name  original file may contain empty line or comment line
 * @param output_file_name file name to be written clean org file name
 * @return output file containing input file content without comment or empty line
 */
FILE* create_clean_file(char* input_file_name, char* output_file_name);

/**
 * Skip spaces before string
 *
 * @param str string with spaces before
 * @return NULL if new string is empty, otherwise returns the string with no space before
 */
char *skip_spaces(char *str);

/**
 * Skip the first word str points
 *
 * @param str starts of string
 * @return the next word after str
 */
char *skip_word(char *str);

/**
 * Read text from file and place it in string
 *
 * @param fp File to read from
 * @param pos position in the file
 * @param len length of chars to read from file
 * @return string containing the text from the file
 */
char *copy_text_from_file_to_string(FILE *fp, fpos_t *pos, int len);

#endif /*ASSEMBLER_PROJECT_ANALYZE_TEXT_H*/