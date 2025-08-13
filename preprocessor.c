#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "preprocessor.h"
#include "utils.h"
#include "analyze_text.h"
#include "parser.h"
#include "errors_handler.h"


char *saved_words[] = {"data", "string", "mat", "entry", "extern","mcroend","mcro",NULL};



int check_as_file_ending(char *file_name)
{
    char *c;

    /* cuts the exising ending of a file  */
    c = strrchr(file_name, '.');
    if (strcmp(c, ".as") == 0){
        return 0;
    }
    return -1;
}

int preprocessor_first_pass(char *org_file_name,char *file_name, int *line_counter , node **head){

    FILE *fp;
    fpos_t pos;
    node *is_mcro_exist = NULL;
    char line[MAX_LINE_LENGTH] ={0},*mcro_name = NULL, *mcro_content = NULL;
    int starting_mcro, found=0,mcroend_without_mcro = 0;

    /*open clean file for reading*/
    fp = fopen(file_name,"r");

    if (fp == NULL) {
        error_log(org_file_name,*line_counter,FILE_NOT_OPEN_READING);
        return -1;
    }

    /* starts reading lines*/
    while (fgets(line, sizeof(line), fp)) {
        (*line_counter)++;

        /* check if line is too long*/
        if (strlen(line) >= MAX_LINE_LENGTH - 1) {
            error_log(org_file_name, *line_counter, LONG_LINE);
            fclose(fp);
            return -1;
        }

       /*check for mcroend without mcro start*/
        if (strncmp(line, MCROEND, 7) == 0) {
            if(mcroend_without_mcro != 1)
            {
                error_log(org_file_name,*line_counter,MACROEND_WITHOUT_START);
                fclose(fp);
                return -1;

            }
            mcroend_without_mcro = 0;
        }

        /* first word in the line is mcro*/
        if (strncmp(line, MCRO, 4) == 0) {
            /*mcroend after mcro, needs to ignore*/

            if (strncmp(line, MCROEND, 7) != 0) {
                mcro_name = cut_spaces_before_and_after_string(line);
                if (mcro_name == NULL) {
                    error_log(org_file_name, *line_counter, MACRO_WITHOUT_NAME);
                    return -1;
                }

                mcroend_without_mcro = 1;
                starting_mcro = *line_counter;

                /* extract mcro name*/
                mcro_name = identify_macro_name(org_file_name, line, line_counter);
                if (mcro_name == NULL) {
                    fclose(fp);
                    return -1;
                }
                fgetpos(fp, &pos);

                /* extract mcro text*/
                mcro_content = extract_mcro_text(org_file_name,fp, &pos, line_counter);
                if (mcro_content == NULL) {
                    free(mcro_name);
                    fclose(fp);
                    return -1;
                }
                fsetpos(fp, &pos);

                /* before adding mcro to linked list check if already exist*/
                is_mcro_exist = search_node_in_linked_list(*head, mcro_name, &found);

                /* mcro name exist already*/
                if (found) {

                    /* different definitions for the same mcro name*/
                    if (strcmp(is_mcro_exist->text, mcro_content) != 0) {
                        error_log(org_file_name, *line_counter, MACRO_MULTI_DEF);
                        free(mcro_name);
                        free(mcro_content);
                        fclose(fp);
                        return -1;
                    } else {
                        /*mcro exist therefore no need to add to list, continue to the next line*/
                        free(mcro_name);
                        free(mcro_content);
                    }

                } else {
                    /*add mcro to the macros linked list*/
                    add_node_to_linked_list(head, mcro_name, mcro_content, starting_mcro);
                }
            }
        }
    }

    fclose(fp);
    return 0;
}

char *identify_macro_name(char *org_file_name, char *line, int *line_counter){

    char *mcro_name, *extra_text_after_mcro_name;

    /* skip mcro word to reach mcro name*/
    mcro_name = skip_one_word(line);

    /* no mcro name exist*/
    if(mcro_name == NULL)
    {
        error_log(org_file_name,*line_counter,MACRO_WITHOUT_NAME);
        return NULL;
    }
    /* check for extra text after mcro name*/
    extra_text_after_mcro_name= skip_one_word(mcro_name);
    if(extra_text_after_mcro_name !=NULL)
    {
        error_log(org_file_name,*line_counter,EXTRA_TEXT_AFTER_MCRO_START);
        return NULL;
    }

    /* check mcro name validity*/
    if (!mcro_name_validation(mcro_name))
    {
        error_log(org_file_name,*line_counter,ILLEGAL_MACRO_NAME);
        return NULL;
    }

    return mcro_name;

}

int mcro_name_validation(char *mcro_name){

    char temp_name[MAX_LINE_LENGTH];
    /* temp name for safety*/
    strncpy(temp_name, mcro_name, strlen(mcro_name) - 1);
    temp_name[strlen(mcro_name) - 1] = '\0';

    /* return validation result*/
    return (mcro_name_only_letters_num_underscore(temp_name) && identify_opcode(temp_name) && identify_register(temp_name)
    && identify_directive(temp_name) && is_save_word(temp_name));

}

int mcro_name_only_letters_num_underscore(char *mcro_name){

    int i;
    for (i = 0; i < strlen(mcro_name); i++) {
        if (!isalpha(mcro_name[i]) && !isdigit(mcro_name[i]) && mcro_name[i]!='_') {
            return 0; /*mcro name contain forbidden char*/
        }
    }

    return 1;
}

int is_save_word(char *mcro_name){

    /*check if mcro_name is one of the forbidden saved word*/
    char mcro_name_lower[MAX_LINE_LENGTH];
    int i;

    for (i = 0; mcro_name[i] != '\0'; i++) {
        mcro_name_lower[i] = tolower(mcro_name[i]);
    }
    mcro_name_lower[i] = '\0';

    for (i = 0; saved_words[i] != NULL; i++) {
        if (strcmp(mcro_name_lower, saved_words[i]) == 0) {
           return 0;
        }
    }

    return 1;
}

char *extract_mcro_text(char *org_file_name , FILE *fp, fpos_t *pos, int *line_counter) {
    char line[MAX_LINE_LENGTH]={0};
    char *mcro;
    int mcro_length = 0, internal_line_counter;


    internal_line_counter = *line_counter;
    /* return to first position */
    if (fsetpos(fp, pos) != 0) {
        error_log(org_file_name,*line_counter,FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        internal_line_counter++;

        /* check if mcro text is more than 80 chars*/
        if (strlen(line) >= MAX_LINE_LENGTH - 1) {
            error_log(org_file_name, internal_line_counter, LONG_LINE);
            return NULL;
        }

        if (strncmp(line, MCROEND, 7) == 0) {
            /*check for extra text after mcroend*/
            if (strncmp(line, MCROEND, 8) != 0){
                error_log(org_file_name,internal_line_counter,EXTRA_TEXT_AFTER_MCROEND);
                return NULL;
            }

            /* reach to mcroend*/
            break;
        }
        mcro_length += strlen(line);
    }

    /* extract mcro text*/
    mcro = copy_text_from_file_to_string(fp, pos, mcro_length);

    *line_counter = internal_line_counter-1;

    return mcro;
}


int preprocessor_second_pass(char *org_file_name,node **head,char *as_file_name, int *line_counter, char *am_file_name,int total_line_num){

    FILE *fp_as, *fp_am;
    char line[MAX_LINE_LENGTH] ={0};
    int found,i;
    node *macro_node;

    /*open clean file for reading*/
    fp_as = fopen(as_file_name,"r");
    if (fp_as == NULL) {
        error_log(org_file_name,*line_counter,FILE_NOT_OPEN_READING);
        return -1;
    }

    /*open am file for writing*/
    fp_am = fopen(am_file_name,"w");
    if (fp_am == NULL) {
        error_log(org_file_name,*line_counter,FILE_NOT_OPEN_WRITING);
        /* close files */
        fclose(fp_as);
        return -1;
    }

    for (i = 0; i < total_line_num ; i++) {
        fgets(line, sizeof(line), fp_as);
        if (line != NULL){
            if (strncmp(line, MCRO, 5) == 0) {
                /* skip lines until mcroend */
                while (fgets(line, sizeof(line), fp_as) != NULL) {
                    if (strncmp(line, MCROEND, 7) == 0) {
                        fgets(line, sizeof(line), fp_as);
                        break;
                    }
                }
            }
            /* search for mcro define in linked list*/
            macro_node = search_node_in_linked_list(*head, line, &found);

            if (found) {
                /*mcro name will be switched with definition */
                fputs(macro_node->text, fp_am);
            } else {
                /* regular line will be copied to am file */
                fputs(line, fp_am);

            }
        }
    }


    fclose(fp_as);
    fclose(fp_am);



    return 0;
}



int preprocessor_full_flow(char *file_name,node **head){

    FILE  *first_copy;
    char *am_file_name, clean_file_name[MAX_LINE_LENGTH];
    int indication, line_counter = 0,total_line_num;

    indication = 0;

    if(check_as_file_ending(file_name)!=0){
        error_log(file_name,line_counter,ARG_NOT_AS_FILE);
        return 1;
    }

    sprintf(clean_file_name, "%s_clean.as", file_name);

    /* create clean file with no comment line or spaces*/
    first_copy = create_clean_file(file_name, clean_file_name);
    if (first_copy == NULL) {
        error_log(file_name,line_counter,FAIL_CLEAN_FILE);
        return 1;
    }

    /* map mcro definitions */
    indication = preprocessor_first_pass(file_name,clean_file_name,&line_counter, head);
    if (indication){
        /* release mcro linked list, delete files */
        remove(clean_file_name);
        return 1;
    }

    am_file_name = change_ending_of_file(file_name, ".am");
    if (am_file_name == NULL) {
        remove(clean_file_name);
        return 1;
    }

    total_line_num = line_counter;

    line_counter = 0;
    /*replace mcro call by its text*/
    indication = preprocessor_second_pass(am_file_name,head,clean_file_name,&line_counter,am_file_name,total_line_num);

    if (indication){
        /* release mcro linked list, delete files */
        /*free_linked_list(head);*/
        free(am_file_name);
        remove(clean_file_name);
        return 1;
    }

    /* release mcro linked list, delete and close files */
    /*free_linked_list(head);*/
    free(am_file_name);
    remove(clean_file_name);

    return 0;
}