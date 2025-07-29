#ifndef ASSEMBLER_PROJECT_LINKED_LIST_H
#define ASSEMBLER_PROJECT_LINKED_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/*
 * Node in a linked list for macros
 * Containing:
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


/**
 * @brief create node and fill the relevant data
 *
 * @param name name of the macro
 * @param text macro text
 * @param line_num line number of the start of the macro
 * @return new pointer of the new node
 */
node *create_node(char *name, char *text, int line_num);

/**
 * @brief add node to exising linked list
 *
 * @param head the first node of the linked list
 * @param name name of the required macro
 * @param text macro text
 * @param line_num line number of the start of the macro
 */
void add_node_to_linked_list(node **head, char *name, char *text, int line_num);

/**
 * @brief search macro in exising linked list
 *
 * @param head the first node of the linked list
 * @param name name of the required macro
 * @param found indicate if the macro already exists
 * @return the required node if found.
 *         NULL if the list is empty.
 *         the previous node if node does not exist.
 */
node *search_node_in_linked_list(node *head, char *name, int *found);

/**
 * @brief Frees memory allocated for a node.
 *
 * This function deallocates memory from a node of a macro
 * @param node a pointer to the node to be free
 */
void free_node(node *node);

/**
 * @brief Frees memory allocated for whole linked list.
 *
 * @param head the first node of the linked list
 */
void free_linked_list(node *head);

#endif /*ASSEMBLER_PROJECT_LINKED_LIST_H*/
