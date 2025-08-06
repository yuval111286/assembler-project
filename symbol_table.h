
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h" 

/* 
 * Structure representing a symbol (label) in the program.
 * Each node is part of a linked list in the symbol table.
 */
typedef struct Symbol {
    char name[MAX_LABEL_LEN + 1]; /* Label name  */
    int address;      /* Memory address (IC or DC value) */
    SymbolType type; /* Type of symbol: CODE / DATA / EXTERN */
    int is_entry;    /* Boolean flag 1 if .entry, 0 for extern */
    struct Symbol *next;  /* Pointer to next symbol in the list */
} Symbol;

/*
 * The SymbolTable structure holds the head of the linked list.
 */
typedef struct {
    Symbol *head; /* Pointer to the first symbol (NULL if empty) */
} SymbolTable;

/*
 * draft
 */
typedef struct ExternSymbol {
    char symbol_name[MAX_LABEL_LEN + 1];
    int address;
    struct ExternSymbol *next;
} ExternSymbol;

typedef struct {
    ExternSymbol *head;
} ExternList;



/*
 * Initializes the symbol table by setting its head to NULL.
 */
void init_symbol_table(SymbolTable *table);

/*
 * Adds a new symbol to the table.
 * Returns 1 if added successfully, or 0 if symbol already exists.
 */
int add_symbol(SymbolTable *table, char *name, int address, SymbolType type);

/*
 * Checks if a symbol with the given name already exists in the table.
 * Returns 1 if exists, 0 otherwise.
 */
int symbol_exists(SymbolTable *table, char *name);

/*
 * Marks a previously defined symbol as .entry by setting is_entry = 1.
 * Does nothing if the symbol is not found.
 */
void mark_entry_label(SymbolTable *table, char *name);

/*
 * Updates all DATA-type symbols by adding IC_final to their address.
 * Should be called after the first pass is complete.
 */
void update_data_symbols_base_address(SymbolTable *table, int ic_final);

/*
 * Returns a pointer to the symbol with the given name, or NULL if not found.
 * This can be used in second pass to resolve label addresses.
 */
Symbol *get_symbol(SymbolTable *table, char *name);

/*
 * Frees all memory allocated for the symbol table.
 * Should be called at the end of assembly to avoid memory leaks.
 */
void free_symbol_table(SymbolTable *table);

#endif /* SYMBOL_TABLE_H */
