#ifndef ASSEMBLER_PROJECT_LINKED_LIST_H
#define ASSEMBLER_PROJECT_LINKED_LIST_H

#include <stdlib.h>
#include <string.h>

/*
 * Node in a linked list
 * Containing:
 * node name
 * node text -
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
 * @param name name of the node
 * @param text node text
 * @param line_num line number of the start of the required text
 * @return new pointer of the new node
 */
node *create_node(char *name, char *text, int line_num);

/**
 * @brief add node to exising linked list
 *
 * @param head the first node of the linked list
 * @param name name of the required node
 * @param text node text
 * @param line_num line number of the start of the required text
 */
void add_node_to_linked_list(node **head, char *name, char *text, int line_num);

/**
 * @brief search node in exising linked list according to the name
 *
 * @param head the first node of the linked list
 * @param name name of the required node
 * @param found indicate if the name already exists
 * @return the required node if found.
 *         NULL if the list is empty.
 *         the previous node if node does not exist.
 */
node *search_node_in_linked_list(node *head, char *name, int *found);

/**
 * @brief Frees memory allocation for node.
 *
 * @param node a pointer to the node to be free
 */
void free_node(node *node);

/**
 * @brief Frees memory allocation for the linked list.
 *
 * @param head the first node of the linked list
 */
void free_linked_list(node *head);

#endif /*ASSEMBLER_PROJECT_LINKED_LIST_H*/
