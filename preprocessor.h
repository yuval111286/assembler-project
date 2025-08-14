
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "linked_list.h"


/**
 * @brief perform preprocessor pass
 *   extract mcro name and text and replace marco appearance by it's name
 *
 * @param file_name as file name
 * @return 0 if preprocessor succeed or 1 if fail
 */
int preprocessor_full_flow(char *file_name,node **head);


#endif 
