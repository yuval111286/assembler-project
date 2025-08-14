#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

/**
 * @brief Perform memory allocation according to given size using malloc
 *
 * @param size - requested memory type
 * @return pointer to requested data
 */
void *malloc_allocation(size_t size);

/**
 * Gets file name and change it's end to requested ending
 * @param file_name file name for the new file
 * @param new_ending name of the ending of the new file
 * @return new string pointer containing the file name with the new ending
 */
char *change_ending_of_file(char *file_name, char *new_ending);


/**
 * @param str line from file
 * @returns clean string without space in the start and the end of the string.
 */
char *cut_spaces_before_and_after_string(char *str);




#endif