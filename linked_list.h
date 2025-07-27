

#ifndef ASSEMBLER_PROJECT_LINKED_LIST_H
#define ASSEMBLER_PROJECT_LINKED_LIST_H




#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*
 * node in a linked list for macros
 * containing:
 * macro name
 * macro text - between macro to mcroend
 * number of line
 * pointer to the next node in the list
 */
typedef struct node{
    char *name;
    char *text;
    int line;
    struct node *next;
} node;


/*
 * Receive
 * Return new pointer of node
 */
node *create_node(char *name, char *content, int line_num);

/*
 * Receive
 * Return new pointer of node
 */
void add_to_list(node **head, ...);

/*
 * Receive
 * Return new pointer of node
 */
node *search_list(node *head, char *name, int *found);

/*
 * Receive
 * Return new pointer of node
 */
void free_list(node *head);





#endif /*ASSEMBLER_PROJECT_LINKED_LIST_H*/
