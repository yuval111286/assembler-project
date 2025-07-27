

#include "linked_list.h"
#include "utils.h"

node *create_node(char *name, char *content, int line_num) {
    node *new_node;

    new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL) {
       /* print error*/
        return NULL;
    }

    new_node->name = malloc_allocation(strlen(name) + 1);
    strcpy(new_node->name, name);

    new_node->text = malloc_allocation(strlen(content) + 1);
    strcpy(new_node->text, content);

    new_node->line = line_num;
    new_node->next = NULL;

    return new_node;
}