#ifndef ASSEMBLER_PROJECT_LINKED_LIST_H
#define ASSEMBLER_PROJECT_LINKED_LIST_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Node in a linked list.
 *
 * Contains
 * - name: Name of the node.
 * - text: Associated text content.
 * - line: Line number where the text starts.
 * - next: Pointer to the next node in the list.
 */
typedef struct node {
    char *name;       
    char *text;        
    int line;        
    struct node *next; 
} node;

/**
 * @brief Create a new linked list node and fill its data.
 * @param name Name of the node.
 * @param text Text content of the node.
 * @param line_num Line number associated with the node.
 * @return Pointer to the newly created node.
 */
node *create_node(char *name, char *text, int line_num);

/**
 * @brief Add a new node to an existing linked list.
 * @param head Pointer to the head of the linked list.
 * @param name Name of the new node.
 * @param text Text content of the new node.
 * @param line_num Line number associated with the new node.
 */
void add_node_to_linked_list(node **head, char *name, char *text, int line_num);

/**
 * @brief Search for a node in the linked list by name.
 * @param head Pointer to the head of the linked list.
 * @param name Name of the node to search for.
 * @param found Pointer to int that will be set to 1 if found, 0 otherwise.
 * @return If found, returns the node.
 *         If list is empty, returns NULL.
 *         If not found, returns the last node.
 */
node *search_node_in_linked_list(node *head, char *name, int *found);

/**
 * @brief Free the memory allocated for a single node.
 * @param node Pointer to the node to free.
 */
void free_node(node *node);

/**
 * @brief Free the entire linked list.
 * @param head Pointer to the head of the linked list.
 */
void free_linked_list(node *head);

#endif 
