
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>

/* Maximum length of a line in the input file (including newline and null) */
#define MAX_LINE_LENGTH 81

/* Maximum length of a label name */
#define MAX_LABEL_LEN 31

/* Maximum number of operands per instruction */
#define MAX_OPERANDS 2

/* Starting address for instructions (IC) */
#define IC_INIT_VALUE 100

/* Maximum number of registers (r0–r7) */
#define NUM_REGISTERS 8

/* Maximum number of digits in a 10-bit base-4 word: from 00000 to 33333 */
#define WORD_BASE4_DIGITS 5

/* ========== Enums ========== */

/*
 * Represents the type of line in the source file
 */
typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_DIRECTIVE,
    LINE_INSTRUCTION,
    LINE_INVALID
} LineType;

/*
 * Represents the type of symbol stored in the symbol table
 */
typedef enum {
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERN
} SymbolType;

/*
 * Represents the addressing modes used by operands
 */
typedef enum {
    ADDRESS_IMMEDIATE = 0, /* #5       */
    ADDRESS_DIRECT    = 1, /* LABEL    */
    ADDRESS_MATRIX    = 2, /* LABEL[r1][r2] */
    ADDRESS_REGISTER  = 3  /* r0–r7    */
} AddressingMode;

/*
 * Represents all valid opcodes (machine instructions)
 */
typedef enum {
    OPCODE_MOV = 0,
    OPCODE_CMP,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_LEA,
    OPCODE_CLR,
    OPCODE_NOT,
    OPCODE_INC,
    OPCODE_DEC,
    OPCODE_JMP,
    OPCODE_BNE,
    OPCODE_RED,
    OPCODE_PRN,
    OPCODE_JSR,
    OPCODE_RTS,
    OPCODE_STOP,
    OPCODE_INVALID = -1 /* Used for invalid/unrecognized instructions */
} Opcode;


typedef struct {
    char* name;
    Opcode opcode;
} OpcodeEntry;

static const OpcodeEntry opcode_table[] = {
        {"mov", OPCODE_MOV},
        {"cmp", OPCODE_CMP},
        {"add", OPCODE_ADD},
        {"sub", OPCODE_SUB},
        {"lea", OPCODE_LEA},
        {"clr", OPCODE_CLR},
        {"not", OPCODE_NOT},
        {"inc", OPCODE_INC},
        {"dec", OPCODE_DEC},
        {"jmp", OPCODE_JMP},
        {"bne", OPCODE_BNE},
        {"red", OPCODE_RED},
        {"prn", OPCODE_PRN},
        {"jsr", OPCODE_JSR},
        {"rts", OPCODE_RTS},
        {"stop", OPCODE_STOP},
        {NULL, OPCODE_INVALID}
};


typedef enum {
    DATA = 0, /* .data */
    STRING, /* .string */
    MAT, /* .mat */
    ENTRY,  /* .entry */
    EXTERN  /*.extern*/
} Directive;


typedef struct {
    char* name;
    Directive directive;
} Directive_Mode;

static const Directive_Mode directive_table[] = {
        {".data", DATA},
        {".string", STRING},
        {".mat", MAT},
        {".entry", ENTRY},
        {".extern", EXTERN},
};




typedef enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    REGISTER_INVALID = -1 /* Used for invalid/unrecognized register */
} Register;

typedef struct {
    char* name;
    Directive reg;
} Register_Type;

static const Register_Type register_table[] = {
        {"r0", R0},
        {"r1", R1},
        {"r2", R2},
        {"r3", R3},
        {"r4", R4},
        {"r5", R5},
        {"r6", R6},
        {"r7", R7},
};


/* ========== Structures ========== */

/*
 * ParsedLine represents the result of parsing a single line from the source file.
 * It stores metadata used by first pass and second pass of the assembler.
 */
typedef struct {
    char label[MAX_LABEL_LEN + 1];              /* Optional label (if exists), or empty string */
    LineType line_type;                         /* Type of line: directive, instruction, etc. */
    Opcode opcode;                              /* Opcode if this is an instruction */
    char directive_name[8];                     /* Name of directive (e.g., "data", "string") */
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH]; /* Up to two operands (as strings) */
    int operand_count;                          /* Number of operands found */
    int line_number;                            /* Line number in the original file (for errors) */
} ParsedLine;



/**
 * @brief Source file location information
 * @details Structure to identify line in source file,
 *          containing file name and line number
 */
typedef struct {
    char *file_name;  /*Name of the source file*/
    int line;         /* Line number in the file*/
} SourceFileLocation;

#endif /* GLOBALS_H */
