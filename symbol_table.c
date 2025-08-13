#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "errors_handler.h"


void init_symbol_table(SymbolTable *table) {
    (*table).head = NULL; /* Set head of symbol list to NULL */
}


int add_symbol(SymbolTable *table, char *name, int address, SymbolType type) {
    Symbol *current;

    current = (*table).head;

    /* Check if the symbol already exists */
    while (current != NULL) {
        if (strcmp((*current).name, name) == 0) {
            return 0; /* Symbol already exists – do not add again */
        }
        current = (*current).next;
    }

    /* Allocate memory for the new symbol */
    current = (Symbol *)malloc(sizeof(Symbol));
    if (current == NULL) {
        error_log(INTERNAL,0,MEMORY_FAIL);
        return 1; /* Abort on critical memory error */
    }

    /* Set all fields of the new symbol */
    strncpy((*current).name, name, MAX_LABEL_LEN); /* Copy label name */
    (*current).name[MAX_LABEL_LEN] = '\0';    /* Ensure null-termination */
    (*current).address = address;    /* Assign address */
    (*current).type = type;   /* Assign symbol type */
    (*current).is_entry = 0;    /* Initially not an entry */
    (*current).next = (*table).head;     /* Link to previous head */
    (*table).head = current;   /* Set new head of list */

    return 1; 
}


int symbol_exists(SymbolTable *table, char *name) {
    Symbol *current;

    /* The head of the symbol table */
    current = (*table).head;

    /* Move the linked list of symbols */
    while (current != NULL) {
        /* Compare current symbol's name to the target name */
        if (strcmp((*current).name, name) == 0) {
            return 1; 
        }
        current = (*current).next; 
    }

    return 0; 
}

void mark_entry_label(SymbolTable *table, char *name) {
    Symbol *current;

    /* Start from the head of the symbol table */
    current = (*table).head;

    /* Move in the linked list */
    while (current != NULL) {
        /* If symbol name matches, mark it as entry */
        if (strcmp((*current).name, name) == 0) {
            (*current).is_entry = 1; /* Mark as entry */
            return; 
        }
        current = (*current).next; 
    }
}

void update_data_symbols_base_address(SymbolTable *table, int ic_final) {
    Symbol *current;

    /* Start from the head of the symbol table */
    current = (*table).head;

    /* Traverse the linked list */
    while (current != NULL) {
        /* Update address for data symbols only */
        if ((*current).type == SYMBOL_DATA) {
            (*current).address += ic_final; 
        }
        current = (*current).next; 
    }
}

Symbol *get_symbol(SymbolTable *table, char *name) {
    Symbol *current;

    /* Start from the head of the symbol table */
    current = (*table).head;

    /* Traverse the linked list */
    while (current != NULL) {
        /* If symbol matches the given name, return it */
        if (strcmp((*current).name, name) == 0) {
            return current; 
        }
        current = (*current).next; 
    }

    
    return NULL; 
}

void free_symbol_table(SymbolTable *table) {
    Symbol *current;
    Symbol *temp;

    /* Start from the head of the symbol table */
    current = (*table).head;

    /* Free each symbol node */
    while (current != NULL) {
        temp = current;                 
        current = (*current).next;      
        free(temp);                      
    }

    /* Set the table head to NULL */
    (*table).head = NULL;
}
