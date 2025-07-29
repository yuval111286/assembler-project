
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "errors_handler.h"
#include "first_pass.h"
#include "preprocessor.h"

int main(int argc, char *argv[])
{

    char *as_file;
    while (--argc > 0){

        as_file = change_ending_of_file(argv[argc],".as");

        printf("-- PREPROCESSOR --\n");
        if (!preprocessor_full_flow(as_file)) {
            continue;
        }

    }


    return 0;
}
