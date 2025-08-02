#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "symbol_table.h"
#include "parser.h"
#include "utils.h"
#include "errors_handler.h"
#include "analyze_text.h"

/* Global definition of the data image array */
unsigned int data_image[MAX_DATA_SIZE];

/**
 * @brief Performs the first pass over a preprocessed .am assembly file.
 *
 * This function scans each line of the input file, detects labels, instructions,
 * and directives, builds the symbol table, and updates IC/DC counters accordingly.
 * It also saves data values into the global data_image[] array for .data/.string/.mat.
 *
 * @param file_name The name of the .am file to process.
 * @param symbol_table Pointer to the symbol table.
 * @param IC_final Pointer to store final instruction counter value.
 * @param DC_final Pointer to store final data counter value.
 * @return 1 on success, 0 on failure (e.g., file read error or parsing issue).
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int IC = IC_INIT_VALUE; /* Initialize instruction counter */
    int DC = 0;             /* Initialize data counter */
    ParsedLine parsed;
    int words;

    /* Attempt to open file for reading */
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        error_log(file_name, 0, FILE_NOT_OPEN_READING); /* Log file open error */
        return 0;
    }

    /* Process file line-by-line */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        line_number++; /* Increment line counter */

        /* Skip empty lines or comment lines */
        if (check_line_comment_or_empty(line))
        {
            continue;
        }

        /* Parse the current line */
        if (!parse_line(line, &parsed))
        {
            error_log(file_name, line_number, SYNTAX_ERROR); /* Log syntax error */
            continue;
        }

        /* Handle label definitions */
        if (parsed.label[0] != '\0')
        {
            if (!is_valid_label(parsed.label))
            {
                error_log(file_name, line_number, INVALID_LABEL_NAME);
                continue;
            }
            else if (identify_opcode(parsed.label) != OPCODE_INVALID ||
                     identify_directive(parsed.label) != -1 ||
                     identify_register(parsed.label) != -1)
            {
                error_log(file_name, line_number, LABEL_RESERVED_WORD);
                continue;
            }
            else if (symbol_exists(symbol_table, parsed.label))
            {
                error_log(file_name, line_number, DUPLICATE_LABEL);
                continue;
            }
            else if (parsed.line_type == LINE_INSTRUCTION)
            {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE))
                {
                    error_log(file_name, line_number, FAILED_ADD_INSTRUCTION_LABEL);
                    continue;
                }
            }
            else if (parsed.line_type == LINE_DIRECTIVE &&
                     (strcmp(parsed.directive_name, "data") == 0 ||
                      strcmp(parsed.directive_name, "string") == 0 ||
                      strcmp(parsed.directive_name, "mat") == 0))
            {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA))
                {
                    error_log(file_name, line_number, FAILED_ADD_DATA_LABEL);
                    continue;
                }
            }
            else if (strcmp(parsed.directive_name, "extern") == 0)
            {
                error_log(file_name, line_number, ILLEGAL_EXTERN_LABEL);
                continue;
            }
        }

        /* Handle instructions */
        if (parsed.line_type == LINE_INSTRUCTION)
        {
            words = instruction_word_count(&parsed);
            if (words > 0)
            {
                IC += words; /* Update instruction counter */
            }
            else
            {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
            }
        }

        /* Handle directives */
        if (parsed.line_type == LINE_DIRECTIVE)
        {
            if (strcmp(parsed.directive_name, "data") == 0)
            {
                int i;
                for (i = 0; i < parsed.operand_count; i++)
                {
                    if (DC >= MAX_DATA_SIZE) /* Check for data overflow */
                    {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        continue;
                    }
                    data_image[DC++] = atoi(parsed.operands[i]); /* Store integer */
                }
            }
            else if (strcmp(parsed.directive_name, "string") == 0)
            {
                char *s = parsed.operands[0];
                int i;
                for (i = 0; s[i] != '\0'; i++)
                {
                    if (DC >= MAX_DATA_SIZE) /* Check for overflow */
                    {
                        error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                        continue;
                    }
                    data_image[DC++] = (unsigned int)s[i]; /* Store character */
                }
                if (DC >= MAX_DATA_SIZE) /* Check before null terminator */
                {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    continue;
                }
                data_image[DC++] = 0; /* Add null terminator */
            }
            else if (strcmp(parsed.directive_name, "mat") == 0)
            {
                int mat_rows, mat_cols;
                int i, values_expected;

                mat_rows = atoi(parsed.operands[0]); /* Parse rows */
                mat_cols = atoi(parsed.operands[1]); /* Parse columns */

                if (mat_rows <= 0 || mat_cols <= 0)
                {
                    error_log(file_name, line_number, INVALID_MATRIX_DIMENSIONS);
                    continue;
                }

                values_expected = mat_rows * mat_cols;

                if (parsed.operand_count != 2 + values_expected)
                {
                    error_log(file_name, line_number, MATRIX_VALUE_COUNT_MISMATCH);
                    continue;
                }

                if (DC + 2 + values_expected >= MAX_DATA_SIZE) /* Check for overflow */
                {
                    error_log(file_name, line_number, DATA_IMAGE_OVERFLOW);
                    continue;
                }

                data_image[DC++] = mat_rows; /* Store rows */
                data_image[DC++] = mat_cols; /* Store cols */

                for (i = 2; i < parsed.operand_count; i++)
                {
                    data_image[DC++] = atoi(parsed.operands[i]); /* Store values */
                }
            }
            else if (strcmp(parsed.directive_name, "extern") == 0)
            {
                if (parsed.operand_count != 1)
                {
                    error_log(file_name, line_number, EXTERN_SYNTAX_ERROR);
                }
                else if (!add_symbol(symbol_table, parsed.operands[0], 0, SYMBOL_EXTERN))
                {
                    error_log(file_name, line_number, DUPLICATE_EXTERN);
                }
            }
            /* .entry directives are handled during the second pass */
        }
    }

    fclose(fp); /* Close the source file */

    *IC_final = IC; /* Save final instruction counter */
    *DC_final = DC; /* Save final data counter */

    update_data_symbols_base_address(symbol_table, IC); /* Adjust data labels */

    return 1; /* Success */
}
