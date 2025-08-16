#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>

/*Number of bits in a machine word*/
#define BITS_IN_WORD 10

/*Maximum positive number 511*/
#define MAX_NUM ((1 << (BITS_IN_WORD-1)) - 1)

/*Minimum negative number 512*/
#define MIN_NUM (-(1 << (BITS_IN_WORD-1)))

/* Maximum length of a name of input file */
#define MAX_FILE_NAME_LENGTH 70

/* Maximum length of a line in the input file  */
#define MAX_LINE_LENGTH 81

/* Maximum length of a label name */
#define MAX_LABEL_LEN 31

/* Maximum number of operands per instruction */
#define MAX_OPERANDS 10

/* Starting address for instructions IC */
#define IC_INIT_VALUE 100

/* Maximum number of registers r0–r7 */
#define NUM_REGISTERS 8

/* Maximum number of directive options */
#define NUM_DIRECTIVE 5

/* Maximum number of digits in a 10bit base4 word: from 00000 to 33333 */
#define WORD_BASE4_DIGITS 5

/* Maximum code and data section sizes */
#define MAX_CODE_SIZE 255
#define MAX_DATA_SIZE 255


/* Groups of bit in encoded word*/
#define ARE_BITS 0 /* 0-1 */
#define DEST_BITS 2 /* 2-3 */
#define SRC_BITS 4 /* 4-5 */
#define OPCODE_BITS 6 /* 6-9 */

#define FOUR_BITS_MASK 0xF /* Moving 4 places */
#define TWO_BITS_MASK  0x3 /* Moving 2 places */


/*ARE*/
#define ABSOLUTE 0 /* 00 */
#define EXTERNAL 1 /* 01 */
#define RELOCATABLE 2 /* 10 */

/* Type of line in source file */
typedef enum {
    LINE_DIRECTIVE, /*Directive line*/
    LINE_INSTRUCTION, /*Instruction line*/
    LINE_INVALID /*Invalid line*/
} LineType;

/* Type of symbol in symbol table */
typedef enum {
    SYMBOL_CODE, /*Symbol in code section*/
    SYMBOL_DATA, /*Symbol in data section*/
    SYMBOL_EXTERN /*External symbol*/
} SymbolType;

/* Addressing modes */
typedef enum {
    ADDRESS_IMMEDIATE = 0, /*Immediate addressing (#5)*/
    ADDRESS_DIRECT    = 1, /*Direct addressing (LABEL)*/
    ADDRESS_MATRIX    = 2, /*Matrix addressing (M[r1][r2])*/
    ADDRESS_REGISTER  = 3 /*Register addressing (r1)*/
} AddressingMode;

/* Opcodes according to manual*/
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
    OPCODE_JSR,
    OPCODE_RED,
    OPCODE_PRN,
    OPCODE_RTS,
    OPCODE_STOP,
    OPCODE_INVALID = -1
} Opcode;

/* Opcode mapping */
typedef struct {
    char *name; /*Instruction name*/
    Opcode opcode; /*Num code*/
} OpcodeEntry;

/* Directive types */
typedef enum {
    DATA = 0, /*Data directive (.data)*/
    STRING,  /*String directive (.string)*/
    MAT,     /*Matrix directive (.mat)*/
    ENTRY,   /*Entry directive (.entry)*/
    EXTERN   /*External directive (.extern)*/
} Directive;

/* Directive name to enum mapping */
typedef struct {
    char *name; /*Directive name*/
    Directive directive; /*Num code*/
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

/* Register name to enum mapping */
typedef struct {
    char *name;   /*Register name ("r0", "r1")*/
    Register reg; /*Reg num*/
} Register_Type;

/* ARE field types for machine code words */
typedef enum {
    ARE_ABSOLUTE = 'A', /* 00 */
    ARE_RELOCATABLE = 'R', /* 10 */
    ARE_EXTERNAL = 'E' /* 01 */
} AREType;

/* setting code word and code image struct */

typedef struct {
    int address; /*Memory address*/
    unsigned int value;  /*Encoded value (10 bits)*/
    char ARE;  /*A,R,E field ('A', 'R', 'E')*/
} CodeWord;

/* Code image storage for machine code words */
typedef struct {
    CodeWord words[MAX_CODE_SIZE]; /*Array of code words*/
    int size;    /*Current number of words*/
} CodeImage;


/* Parsed Line Struct */
typedef struct {
    char label[MAX_LABEL_LEN + 1]; /*Label*/
    LineType line_type; /*Line type*/
    Opcode opcode; /*Operation code*/
    char directive_name[8]; /*Directive name*/
    char operands[MAX_OPERANDS][MAX_LINE_LENGTH];/*Operand list*/
    int operand_count;/*Number of operands*/
    int line_number;/*Line number in file*/
} ParsedLine;

#endif 
