#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h" 

/* Initializes the symbol table to an empty list */
void init_symbol_table(SymbolTable *table) {
    (*table).head = NULL; /* Set head of symbol list to NULL */
}

/* Adds a new symbol to the symbol table */
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
        fprintf(stderr, "Error: Memory allocation failed for symbol\n");
        exit(1); /* Abort on critical memory error */
    }

    /* Set all fields of the new symbol */
    strncpy((*current).name, name, MAX_LABEL_LEN); /* Copy label name */
    (*current).name[MAX_LABEL_LEN] = '\0';    /* Ensure null-termination */
    (*current).address = address;    /* Assign address */
    (*current).type = type;   /* Assign symbol type */
    (*current).is_entry = 0;    /* Initially not an entry */
    (*current).next = (*table).head;     /* Link to previous head */
    (*table).head = current;   /* Set new head of list */

    return 1; /* Symbol added successfully */
}

/* Checks if a symbol already exists in the table */
int symbol_exists(SymbolTable *table, char *name) {
    Symbol *current;

    current = (*table).head;

    while (current != NULL) {
        if (strcmp((*current).name, name) == 0) {
            return 1; /* Symbol found */
        }
        current = (*current).next;
    }

    return 0; /* Symbol not found */
}

/* Marks a symbol as .entry (sets is_entry = 1) */
void mark_entry_label(SymbolTable *table, char *name) {
    Symbol *current;

    current = (*table).head;

    while (current != NULL) {
        if (strcmp((*current).name, name) == 0) {
            (*current).is_entry = 1; /* Mark as entry */
            return; /* Done */
        }
        current = (*current).next;
    }

    /* If not found, do nothing – might be handled later in second pass */
}

/* Adds IC final value to all data symbols after first pass */
void update_data_symbols_base_address(SymbolTable *table, int ic_final) {
    Symbol *current;

    current = (*table).head;

    while (current != NULL) {
        if ((*current).type == SYMBOL_DATA) {
            (*current).address += ic_final; /* Adjust address */
        }
        current = (*current).next;
    }
}

/* Retrieves a pointer to a symbol by name (or NULL if not found) */
Symbol *get_symbol(SymbolTable *table, char *name) {
    Symbol *current;

    current = (*table).head;

    while (current != NULL) {
        if (strcmp((*current).name, name) == 0) {
            return current; /* Return found symbol */
        }
        current = (*current).next;
    }

    return NULL; /* Symbol not found */
}

/* Frees all memory used by the symbol table */
void free_symbol_table(SymbolTable *table) {
    Symbol *current;
    Symbol *temp;

    current = (*table).head;

    while (current != NULL) {
        temp = current;       /* Save current node */
        current = (*current).next;    /* Move to next */
        free(temp);    /* Free saved node */
    }

    (*table).head = NULL; /* Table is now empty */
}
