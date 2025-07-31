#include <stdio.h>              
#include <stdlib.h>             
#include <string.h>             

#include "globals.h"            
#include "symbol_table.h"       
#include "parser.h"            
#include "utils.h"              
#include "errors_handler.h"     
#include "analyze_text.h"      
/**
 * @brief Executes the first pass over a preprocessed .am assembly file.
 *
 * Parses each line, identifies labels, instructions and directives,
 * builds the symbol table and updates IC/DC counters.
 *
 * @param file_name The name of the .am file to process
 * @param symbol_table Pointer to the symbol table
 * @param IC_final Pointer to store final instruction counter
 * @param DC_final Pointer to store final data counter
 * @return 1 if successful, 0 if errors occurred
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

    fp = fopen(file_name, "r");   /* Open the file for reading */
    if (fp == NULL)                        
    {
        error_log(file_name, 0, FILE_NOT_OPEN_READING);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) /* Read each line from the file */
    {
        line_number++;  /* Update line number */

        if (is_empty_or_comment(line))      /* Skip comments and empty lines */
        {
            continue;
        }

        if (!parse_line(line, &parsed))   /* If parsing fails, report syntax error */
        {
            error_log(file_name, line_number, "Syntax error");
            continue;
        }

        /* Handle Label Definitions  */
        if (parsed.label[0] != '\0')    /* If the line has a label */
        {
            if (symbol_exists(symbol_table, parsed.label)) /* Check for duplicates */
            {
                error_log(file_name, line_number, "Label already defined");
            }
            else if (parsed.line_type == LINE_INSTRUCTION) /* Label on instruction */
            {
                if (!add_symbol(symbol_table, parsed.label, IC, SYMBOL_CODE)) /* Add code label */
                {
                    error_log(file_name, line_number, "Failed to add instruction label");
                }
            }
            else if (parsed.line_type == LINE_DIRECTIVE &&
                     (strcmp(parsed.directive_name, "data") == 0 ||
                      strcmp(parsed.directive_name, "string") == 0 ||
                      strcmp(parsed.directive_name, "mat") == 0)) /* Label on data directive */
            {
                if (!add_symbol(symbol_table, parsed.label, DC, SYMBOL_DATA)) /* Add data label */
                {
                    error_log(file_name, line_number, "Failed to add data label");
                }
            }
            else if (strcmp(parsed.directive_name, "extern") == 0) /* Label on .extern is illegal */
            {
                error_log(file_name, line_number, "Label cannot be defined for .extern");
            }
        }

        /*  Handle Instructions  */
        if (parsed.line_type == LINE_INSTRUCTION)
        {
            words = instruction_word_count(&parsed); /* Count words needed for instruction */
            if (words > 0)
            {
                IC += words;   /* Update instruction counter */
            }
            else
            {
                error_log(file_name, line_number, "Invalid instruction operands");
            }
        }

        /*  Handle Directives */
        if (parsed.line_type == LINE_DIRECTIVE)
        {
            if (strcmp(parsed.directive_name, "data") == 0 ||
                strcmp(parsed.directive_name, "string") == 0 ||
                strcmp(parsed.directive_name, "mat") == 0)
            {
                DC += count_data_items(&parsed);     /* Update data counter accordingly */
            }
            else if (strcmp(parsed.directive_name, "extern") == 0)
            {
                if (parsed.operand_count != 1)       /* .extern must have exactly one operand */
                {
                    error_log(file_name, line_number, "Invalid .extern syntax");
                }
                else if (!add_symbol(symbol_table, parsed.operands[0], 0, SYMBOL_EXTERN)) /* Add extern symbol */
                {
                    error_log(file_name, line_number, "Duplicate .extern definition");
                }
            }
            /* Note: .entry is handled only in the second pass */
        }
    }

    fclose(fp);       /* Close the source file */

    *IC_final = IC;     /* Set final IC and DC values */
    *DC_final = DC;

    update_data_symbols_base_address(symbol_table, IC); /* Adjust all data symbol addresses */

    return 1;                               
}
