#include "linked_list.h"
#include "errors_handler.h"
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

    /* returns pointer to the new node */
    return tmp;
}

void add_node_to_linked_list(node **head, char *name, char *text, int line_num){
    int found;
    node *new, *tmp;
    found = 0;

    /* tmp is the previous node of the new node in the list
     * in the case the macro already exists in the list, temp holds the macro with the same name.
     */
    tmp = search_node_in_linked_list(*head,name,&found);

    /* check if the list contains macro with the same name */
    if(found && strcmp(tmp->text,text) != 0){
        /* found the macro name with different definition */
        internal_error_log(MACRO_MULTI_DEF);
        free(name);
        free(text);
        return;
    }


    if(!found){
        /* meaning the macro does not exist - creating new node for the new macro*/
        new = create_node(name,text,line_num);

        /* in case the list is empty adding the new node to be the head*/
        if(tmp == NULL){
            *head = new;
        }

        /* new node is the last node of the list */
        else{
            tmp->next = new;
        }
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
        printf("Node %s was found in the linked list\n", name);
        return head;
    }

    /* reaching to the end of the list */
    if (head->next == NULL) {
        return head;
    }

    /* continue thw search in the rest of the list */
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