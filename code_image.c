#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "code_image.h"
#include "errors_handler.h"
#include "utils.h"

/**
 * @brief Initializes the code image structure.
 *
 * Sets the number of words in the code image to 0.
 *
 * @param img Pointer to the CodeImage structure to initialize.
 */
void init_code_image(CodeImage *img) {
    (*img).size = 0;
}

/**
 * @brief Adds a new code word to the code image.
 *
 * Verifies that the code image has space before adding the word.
 * If full, logs an error and aborts.
 *
 * @param img Pointer to the CodeImage.
 * @param address The memory address of the word.
 * @param value The 10-bit machine code value.
 * @param are The ARE character ('A', 'R', or 'E').
 */
void add_code_word(CodeImage *img, int address, unsigned int value, char are) {
    if ((*img).size >= MAX_CODE_SIZE) {
        error_log("internal", -1, CODE_IMAGE_OVERFLOW); /* log and abort */
        exit(1);
    }

    (*img).words[(*img).size].address = address;
    (*img).words[(*img).size].value = value;
    (*img).words[(*img).size].ARE = are;
    (*img).size++;
}

/**
 * @brief Writes the contents of the code image and data image to a .ob file.
 *
 * Creates the output file with ".ob" extension and writes the IC and DC sizes,
 * followed by the code and data memory words in base-4 encoding.
 *
 * @param img Pointer to the CodeImage.
 * @param ic_size Final instruction counter.
 * @param dc_size Final data counter.
 * @param data_image Pointer to the data image array.
 * @param file_name Base name of the file (without extension).
 */
void write_code_image_to_ob_file(CodeImage *img, int ic_size, int dc_size, unsigned int *data_image, char *file_name) {
    FILE *fp;
    int i;
    char *ob_name;

    ob_name = change_ending_of_file(file_name, ".ob");

    fp = fopen(ob_name, "w");
    if (fp == NULL) {
        error_log(file_name, 0, FILE_NOT_OPEN_WRITING);
        free(ob_name);
        return;
    }


    /* Write the code image section */
    for (i = 0; i < (*img).size; i++) {

        fprintf(fp, "%s %s\n", turn_address_to_base_4((*img).words[i].address), turn_line_to_base_4((*img).words[i].value));
    }

    /* Write the data image section */
    for (i = 0; i < dc_size; i++) {

        fprintf(fp, "%s %s\n", turn_address_to_base_4(IC_INIT_VALUE + ic_size + i), turn_line_to_base_4(data_image[i]));

    }

    fclose(fp);
    free(ob_name);
}

/**
 * @brief Converts an unsigned integer (0–1023) to a 5-digit base-4 string.
 *
 * Encodes the base-4 digits using 'a', 'b', 'c', 'd' for values 0–3.
 * Returns a static string valid until next call.
 *
 * @param value The number to convert (expected ≤ 1023).
 * @return Pointer to a null-terminated 5-character base-4 string.
*/
char *turn_line_to_base_4(unsigned int value) {
    static char base4[6] = {0};
    char digits[4] = {'a', 'b', 'c', 'd'};
    int i,remainder;

    base4[5] = '\0';

    for (i = 4; i >= 0; i--) {
        remainder = value % 4;

        base4[i] = digits[remainder];
        value = value / 4;
    }

    return base4;
}

char *turn_address_to_base_4(unsigned int value) {
    static char base4[6] = {0};         /* 5 digits + null terminator */
    char digits[4] = {'a', 'b', 'c', 'd'};
    char temp[6];                       /* מערך זמני */
    int i, remainder, start_index;

    /* טיפול במקרה מיוחד של 0 */
    if (value == 0) {
        base4[0] = 'a';
        base4[1] = '\0';
        return base4;
    }

    /* המרה למערך זמני */
    temp[5] = '\0';
    for (i = 4; i >= 0; i--) {
        remainder = value % 4;
        temp[i] = digits[remainder];
        value = value / 4;
    }

    /* מצא את הספרה הראשונה שאינה 'a' (אפס מוביל) */
    start_index = 0;
    while (start_index < 5 && temp[start_index] == 'a') {
        start_index++;
    }

    /* העתק ללא אפסים מובילים */
    strcpy(base4, &temp[start_index]);

    return base4;
}

