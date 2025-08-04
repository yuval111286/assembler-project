
#ifndef PROJECT_PREPROCESSOR_H
#define PROJECT_PREPROCESSOR_H

#include <stdio.h>
#include "linked_list.h"

/*Macros for mcro start and mcroend end*/
#define MCRO "mcro "
#define MCROEND "mcroend\n"

/**
 * @brief perform preprocessor pass
 *   extract mcro name and text and replace marco appearance by it's name
 *
 * @param file_name as file name
 * @return 0 if preprocessor succeed or 1 if fail
 */
int preprocessor_full_flow(char *file_name,node **head);

/**
 *
 * @param file_name
 * @return 0 if file ends with .as, -1 if not
 */
int check_as_file_ending(char *file_name);

/**
 *  @brief go over as file and extract mcro definitions
 *
 * @param org_file_name file name for report errors
 * @param file_name clean file name with no comment or empty lines
 * @param line_counter counter of lines in clean file
 * @param head head of linked list of macros
 * @return 0 for success, -1 for fail
 */
int prepro_first_pass(char *org_file_name, char *file_name, int *line_counter, node **head);

/**
 * @brief go over as file and replace maco call in it's text
 *
 * @param org_file_name file name for report errors
 * @param head head of linked list of macros
 * @param as_file_name clean file name to read from
 * @param line_counter counter of lines in clean file
 * @param am_file_name am file name to write to
 * @return 0 for success, -1 for fail
 */
int preproc_second_pass(char *org_file_name, node **head, char *as_file_name, int *line_counter, char *am_file_name);

/**
 *@brief extract macro name from line and verify if valid name using internal functions
 *
 * @param org_file_name file name for report errors
 * @param line line contains mcro name
 * @param line_counter line counter for report errors
 * @return mcro name for success , NULL if fails
 */
char *identify_macro_name(char *org_file_name, char *line, int *line_counter);

/**
 * @brief check if mcro name contain char different than letter, digit or underscore, instruction, opcode or register
 *
 * @param mcro_name mcro name to be verified
 * @return number indication name validations result
 */
int mcro_name_validation(char *mcro_name);

/**
 *@brief check if mcro name contain char different than letter, digit or underscore)
 *
 * @param mcro_name mcro name to be verified
 * @return 1 if valid name 0 if not
 */
int mcro_name_only_letters_num_underscore(char *mcro_name);

/**
 *@brief check if mcro name is a saved word
 * char *saved_words[] = {"data", "string", "mat", "entry", "extern","mcroend","mcro", NULL};
 *
 * @param mcro_name mcro name to be verified
 * @return 1 if valid name 0 if macro name is one of the saved word
 */
int is_save_word(char *mcro_name);

/**
 *
 * @param org_file_name
 * @param fp file
 * @param pos position in file
 * @param line_counter counter of lines reading from file
 * @return
 */
char *extract_mcro_text(char *org_file_name , FILE *fp, fpos_t *pos, int *line_counter);


#endif /*PROJECT_PREPROCESSOR_H*/
