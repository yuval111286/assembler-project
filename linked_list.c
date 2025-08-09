#include "linked_list.h"
#include "utils.h"


node *create_node(char *name, char *text, int line_num){
    node *tmp;

    /* memory allocation for the node */
    tmp = malloc_allocation(sizeof(node));

    /* node name */
    tmp->name = name;
    /* node string */
    tmp->text = text;
    /* macro line number */
    tmp->line = line_num;
    /* next pointer shall be NULL*/
    tmp->next = NULL;

    /* returns pointer of the new node */
    return tmp;
}

void add_node_to_linked_list(node **head, char *name, char *text, int line_num){
    node *new, *tmp;
    char *new_name, *new_text;

    /* allocate memory for name copy */
    new_name = malloc_allocation(strlen(name) + 1);
    if (new_name == NULL) {
        return;
    }

    /* allocate memory for text copy */
    new_text = malloc_allocation(strlen(text) + 1);
    if (new_text == NULL) {
        free(new_name);
        return;
    }

    /* copy strings to allocated memory */
    strcpy(new_name, name);
    strcpy(new_text, text);

    /* create new node  */
    new = create_node(new_name, new_text, line_num);

    /* check if list is empty */
    if (*head == NULL) {
        /* list is empty so new node becomes the head */
        *head = new;
    } else {
        /* list is not empty so finds the last node and adds the new node */
        tmp = *head;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new;
    }
}

node *search_node_in_linked_list(node *head, char *name, int *found){
    *found = 0;

    /* return NULL for empty list */
    if (head == NULL) {
        return NULL;
    }

    /* node found */
    if (strcmp(name, head->name) == 0) {
        *found = 1;
        return head;
    }

    /* reaching to the end of the list */
    if (head->next == NULL) {
        return head;
    }

    /* continue the search in the rest of the list */
    return search_node_in_linked_list(head->next, name, found);
}


void free_node(node *node){
    /* free memory for the node */
    free(node->name);
    free(node->text);
    free(node);
}

void free_linked_list(node *head){
    /* free memory of the linked list */
    while(head != NULL) {
        node *tmp = head;
        head = head->next;
        free_node(tmp);
    }
}