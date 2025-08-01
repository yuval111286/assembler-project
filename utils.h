
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
 *
 * @param file_name file name for the new file
 * @param new_ending name of the ending of the new file
 * @return new string pointer containing the file name with the new ending
 */
char *change_ending_of_file(char *file_name, char *new_ending);

#endif 
