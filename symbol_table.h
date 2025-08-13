#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "globals.h" 

/**
 * @brief Represents a symbol (label) in the program.
 * Each symbol is part of a linked list in the symbol table.
 */
typedef struct Symbol {
    char name[MAX_LABEL_LEN + 1]; /* Label name */
    int address;    /* Memory address */
    SymbolType type; /* CODE / DATA / EXTERN */
    int is_entry;   /* 1 if .entry, 0 else */
    struct Symbol *next; /* Pointer to next symbol in list */
} Symbol;

/**
 * @brief Holds the head pointer of the symbol list.
 */
typedef struct {
    Symbol *head; 
} SymbolTable;

/**
 * @brief Represents an external symbol reference.
 */
typedef struct ExternSymbol {
    char symbol_name[MAX_LABEL_LEN + 1]; /* Name of external symbol */
    int address;   /* Address where its used */
    struct ExternSymbol *next;  /* Next in extern list */
} ExternSymbol;

/**
 * @brief Holds the head pointer of the extern list.
 */
typedef struct {
    ExternSymbol *head; 
} ExternList;

/**
 * @brief Initialize the symbol table.
 * @param table Pointer to the symbol table.
 */
void init_symbol_table(SymbolTable *table);

/**
 * @brief Add a new symbol to the table.
 * @param table Pointer to the symbol table.
 * @param name Symbol name.
 * @param address Memory address.
 * @param type Symbol type.
 * @return 1 if added, 0 if symbol already exists.
 */
int add_symbol(SymbolTable *table, char *name, int address, SymbolType type);

/**
 * @brief Check if a symbol exists in the table.
 * @param table Pointer to the symbol table.
 * @param name Symbol name to search.
 * @return 1 if exists, 0 otherwise.
 */
int symbol_exists(SymbolTable *table, char *name);

/**
 * @brief Add IC_final to addresses of all DATA symbols.
 * @param table Pointer to the symbol table.
 * @param ic_final Final IC value from first pass.
 */
void update_data_symbols_base_address(SymbolTable *table, int ic_final);

/**
 * @brief Get a symbol by name.
 * @param table Pointer to the symbol table.
 * @param name Symbol name to search.
 * @return Pointer to the symbol or NULL if not found.
 */
Symbol *get_symbol(SymbolTable *table, char *name);

/**
 * @brief Free all memory in the symbol table.
 * @param table Pointer to the symbol table.
 */
void free_symbol_table(SymbolTable *table);

#endif 
