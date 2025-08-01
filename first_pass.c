#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "parser.h"
#include "utils.h"
#include "errors_handler.h"
#include "analyze_text.h"

/**
 * @brief Performs the first pass over a preprocessed .am assembly file.
 *
 * This function scans each line of the input file, detects labels, instructions,
 * and directives, builds the symbol table, and updates IC/DC counters accordingly.
 *
 * @param file_name The name of the .am file to process.
 * @param symbol_table Pointer to the symbol table.
 * @param IC_final Pointer to store final instruction counter value.
 * @param DC_final Pointer to store final data counter value.
 * @return 0 on success, -1 on failure (e.g., file read error or parsing issue).
 */
int first_pass(char *file_name, SymbolTable *symbol_table, int *IC_final, int *DC_final)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int IC = IC_INIT_VALUE;
    int DC = 0;
    ParsedLine parsed;
    int words;

    /* Attempt to open file for reading */
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 1;
    }

    /* Process file line-by-line */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        line_number++;

        /* Parse the current line */
        if (!parse_line(line, &parsed))
        {
            error_log(file_name, line_number, SYNTAX_ERROR);
            continue;
        }

        /* Handle label definitions */
        if (parsed.label[0] != '\0')
        {
            /* Validate the label */
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
                IC += words;
            }
            else
            {
                error_log(file_name, line_number, INVALID_INSTRUCTION_OPERANDS);
            }
        }

        /* Handle directives */
        if (parsed.line_type == LINE_DIRECTIVE)
        {
            if (strcmp(parsed.directive_name, "data") == 0 ||
                strcmp(parsed.directive_name, "string") == 0 ||
                strcmp(parsed.directive_name, "mat") == 0)
            {
                DC += count_data_items(&parsed);
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

    fclose(fp);

    /* Save final IC and DC values */
    *IC_final = IC;
    *DC_final = DC;

    /* Adjust DATA symbols base addresses (DC += IC offset) */
    update_data_symbols_base_address(symbol_table, IC);

    return 0;
}