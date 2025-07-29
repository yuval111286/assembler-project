
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "errors_handler.h"
#include "first_pass.h"
#include "preprocessor.h"

int main(int argc, char *argv[])
{
    char *as_file/*, *am_file*/;
    while (--argc > 0){

        printf("-- PREPROCESSOR --\n");
        if (!preprocessor_full_flow(as_file)) {
            /*If it failed, move to the next file.*/
            continue;
        }

    }

    return 0;
}
