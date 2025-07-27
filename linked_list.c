#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "errors_handler.h"


node *create_node(char *name, char *text, int line_num){
    node *temp;

    /* Memory allocation for the node */
    temp = malloc_allocation(sizeof(node));

    /* Set the name of the node */
    temp->name = name;
    /* Set the string of the node */
    temp->text = text;
    /* Sets the line number of the macro */
    temp->line = line_num;
    /* Sets the next pointer to be NULL*/
    temp->next = NULL;

    /* Returns pointer to the new node */
    return temp;
}

void add_node_to_linked_list(node **head, char *name, char *text, int line_num){
    int found;
    node *new_node, *temp;
    found = 0;

    /* Temp is the previous node of the new node in the list
     * in the case the macro already exists in the list, temp holds the macro with the same name.
     */
    temp = search_node_in_linked_list(*head,name,&found);

    /* If the list already has a macro with the same name */
    if(found && strcmp(temp->text,text) != 0){
        /* same macro name but with different definition */
        internal_error_log(MACRO_MULTI_DEF);
        free(name);
        free(text);
        return;
    }

    /* The macro does not exist - creating new node with the new macro name */
    if(!found){
        new_node = create_node(name,text,line_num);

        /* If the list is empty, add the new node to the head of the list */
        if(temp == NULL){
            *head = new_node;
        }

        /* If the list is not empty, the new node is the last node */
        else{
            temp->next = new_node;
        }
    }
}

node *search_node_in_linked_list(node *head, char *name, int *found){
    *found = 0;

    /* return NULL for empty list */
    if (head == NULL) {
        return NULL;
    }

    /* If the node already exists */
    if (strcmp(name, head->name) == 0) {
        *found = 1;
        printf("Node %s found in the list\n", name);
        return head;
    }

    /* If reaching to the end of the list */
    if (head->next == NULL) {
        return head;
    }

    /* Recursively search the rest of the list */
    return search_node_in_linked_list(head->next, name, found);
}


void free_node(node *node){
    /* Free memory allocated for the name, text and node */
    free(node->name);
    free(node->text);
    free(node);
}

void free_linked_list(node *head){
    /* Free memory for the current linked list using temp node */
    while(head != NULL) {
        node *temp = head;
        head = head->next;
        free_node(temp);
    }
}