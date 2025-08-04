#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>

/* Maximum length of a line in the input file (including newline and null) */
#define MAX_LINE_LENGTH 81

/* Maximum length of a label name */
#define MAX_LABEL_LEN 31

/* Maximum number of operands per instruction */
#define MAX_OPERANDS 10

/* Starting address for instructions (IC) */
#define IC_INIT_VALUE 100

/* Maximum number of registers (r0–r7) */
#define NUM_REGISTERS 8

/* Maximum number of directive options */
#define NUM_DIRECTIVE 5

/* Maximum number of digits in a 10-bit base-4 word: from 00000 to 33333 */
#define WORD_BASE4_DIGITS 5

/* Maximum code and data section sizes */
#define MAX_CODE_SIZE 255
#define MAX_DATA_SIZE 255

/* Global data image – stores encoded .data/.string/.mat values */
extern unsigned int data_image[MAX_DATA_SIZE];

/* ========== Enums ========== */

/* Type of line in source file */
typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_DIRECTIVE,
    LINE_INSTRUCTION,
    LINE_INVALID
} LineType;

/* Type of symbol in symbol table */
typedef enum {
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERN
} SymbolType;

/* Addressing modes */
typedef enum {
    ADDRESS_IMMEDIATE = 0,
    ADDRESS_DIRECT    = 1,
    ADDRESS_MATRIX    = 2,
    ADDRESS_REGISTER  = 3
} AddressingMode;

/* Opcodes */
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
    OPCODE_INVALID = -1
} Opcode;

typedef struct {
    char *name;
    Opcode opcode;
} OpcodeEntry;

/* Directive types */
typedef enum {
    DATA = 0,
    STRING,
    MAT,
    ENTRY,
    EXTERN
} Directive;

typedef struct {
    char *name;
    Directive directive;
} Directive_Mode;

/* Register types */
typedef enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    REGISTER_INVALID = -1
} Register;

typedef struct {
    char *name;
    Register reg;
} Register_Type;


/* ========== Code Word / Code Image ========== */

typedef struct {
    int address;            /* Memory address (starts at 100) */
    unsigned int value;     /* Encoded machine word (10-bit integer) */
    char ARE;               /* 'A', 'R', or 'E' */
} CodeWord;

typedef struct {
    CodeWord words[MAX_CODE_SIZE];
    int size;               /* Number of words stored */
} CodeImage;


/* ========== Parsed Line Struct ========== */

typedef struct {
    char label[MAX_LABEL_LEN + 1];
    LineType line_type;
    Opcode opcode;
    char directive_name[8];
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH];
    int operand_count;
    int line_number;
} ParsedLine;

#endif /* GLOBALS_H */
