
#ifndef PREPROCESSOR_HELPER_H
#define PREPROCESSOR_HELPER_H



/**
 * @brief Skip the first word str points
 * @param word starts of string
 * @return the next word after str
 */
char *skip_one_word(char *word);

/**
 * @brief Read text from file and place it in string
 * @param fp File to read from
 * @param pos position in the file
 * @param len_of_chars_to_copy length of chars to read from file
 * @return string containing the text from the file
 */
char *copy_text_from_file_to_string(FILE *fp, fpos_t *pos, int len_of_chars_to_copy);



/**
 * @brief Create a cleaned version of file without comments and empty lines.
 *
 * @param input_file_name  original file may contain empty line or comment line
 * @param output_file_name file name to be written clean org file name
 * @return output file containing input file content without comment or empty line
 */
FILE* create_clean_file(char* input_file_name, char* output_file_name);




#endif
