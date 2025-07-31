#include "preprocessor.h"
#include "utils.h"
#include "analyze_text.h"
#include "parser.h"
#include "errors_handler.h"

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

int prepro_first_pass(char *org_file_name,char *file_name, int *line_counter , node **head){

    FILE *fp;
    fpos_t pos;
    node *is_mcro_exist = NULL;
    char line[MAX_LINE_LENGTH] ={0},*mcro_name = NULL, *mcro_content = NULL;
    int starting_mcro, found=0;

    fp = fopen(file_name,"r");

    if (fp == NULL) {
        error_log(org_file_name,*line_counter,FILE_NOT_OPEN_READING);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        (*line_counter)++;

        /* check if line is too long*/
        if (strlen(line) >= MAX_LINE_LENGTH - 1) {
            error_log(org_file_name, *line_counter, LONG_LINE);
            return -1;
        }

        /* first word in the line is mcro*/
        if (strncmp(line, MCRO, 5) == 0){
            starting_mcro = *line_counter;
            mcro_name = identify_macro_name(line,file_name,line_counter);
            if (mcro_name == NULL) {
                fclose(fp);
                return -1;
            }
            fgetpos(fp, &pos);
            mcro_content = extract_mcro_text(fp,&pos,line_counter);
            if (mcro_content == NULL) {
                free(mcro_name);
                fclose(fp);
                return -1;
            }
            fsetpos(fp, &pos);

            is_mcro_exist = search_node_in_linked_list(*head,mcro_name,&found);

            if (found){
                if(strcmp(is_mcro_exist->text,mcro_content) != 0){
                    error_log(org_file_name, *line_counter, MACRO_MULTI_DEF);
                    free(mcro_name);
                    free(mcro_content);
                    fclose(fp);
                    return -1;
                }
                else{
                    free(mcro_name);
                    free(mcro_content);
                }

            }
            else{
                add_node_to_linked_list(head,mcro_name,mcro_content,starting_mcro);

            }
        }
    }

    fclose(fp);
    return 0;
}

char *identify_macro_name(char *line, char *file_name, int *line_counter){

    char *mcro_name, *extra_text_after_mcro_name;

    mcro_name = skip_word(line);
    if(mcro_name == NULL)
    {
        error_log(file_name,*line_counter,MACRO_WITHOUT_NAME);
        return NULL;
    }
    extra_text_after_mcro_name= skip_word(mcro_name);
    if(extra_text_after_mcro_name !=NULL)
    {
        error_log(file_name,*line_counter,EXTRA_TEXT_AFTER_MCRO_START);
        return NULL;
    }

    if (!mcro_name_validation(mcro_name))
    {
        error_log(file_name,*line_counter,ILLEGAL_MACRO_NAME);
        return NULL;
    }

    return mcro_name;

}

int mcro_name_validation(char *mcro_name){

    return (mcro_name_only_letters_num_underscore(mcro_name) && identify_opcode(mcro_name) && identify_register(mcro_name) && is_directive(mcro_name));

}

int mcro_name_only_letters_num_underscore(char *mcro_name){

    int i;

    for (i = 0; mcro_name[i] != '\0'; i++) {
        if (!isalpha(mcro_name[i]) && !isdigit(mcro_name[i]) && mcro_name[i]!='_') {
            return 0;
        }
    }

    return 1;
}

char *extract_mcro_text(FILE *fp, fpos_t *pos, int *line_counter) {
    char line[MAX_LINE_LENGTH]={0};
    char *mcro;
    int mcro_length = 0, internal_line_counter;


    internal_line_counter = *line_counter;
    /* return to first position */
    if (fsetpos(fp, pos) != 0) {
        error_log(NULL,*line_counter,FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        /*(*line_counter)++;(*/
        internal_line_counter++;

        if (strncmp(line, MCROEND, 7) == 0) {
            if (strncmp(line, MCROEND, 8) != 0)
            {
                error_log(NULL,internal_line_counter,EXTRA_TEXT_AFTER_MCROEND);
                return NULL;

            }

            break; /* reach to mcroend*/
        }
        mcro_length += strlen(line);
    }

    mcro = copy_text_from_file_to_string(fp, pos, mcro_length);

    return mcro;
}


int preproc_second_pass(char *org_file_name,node **head,char *as_file_name, int *line_counter, char *am_file_name){

    FILE *fp_as, *fp_am;
    char line[MAX_LINE_LENGTH] ={0};
    int found;
    node *macro_node;


    fp_as = fopen(as_file_name,"r");
    if (fp_as == NULL) {
        error_log(org_file_name,0,FILE_NOT_OPEN_READING);
        return -1;
    }

    fp_am = fopen(am_file_name,"w");
    if (fp_am == NULL) {
        error_log(org_file_name,0,FILE_NOT_OPEN_WRITING);
        /* close files */
        fclose(fp_as);
        return -1;
    }

    while (fgets(line, sizeof(line), fp_as) != NULL) {
        if (strncmp(line, MCRO, 5) == 0) {
            /* skip lines until mcroend */
            while (fgets(line, sizeof(line), fp_as) != NULL) {
                if (strncmp(line, MCROEND, 7) == 0) {
                    fgets(line, sizeof(line), fp_as);
                    break;
                }
            }
        }

        macro_node = search_node_in_linked_list(*head, line, &found);

        if (found) {
            /*mcro name will be switched with definition */
            fputs(macro_node->text, fp_am);
        } else {
            /* regular line will be copied to am file */
            fputs(line, fp_am);
        }
    }

    /* close files*/
    fclose(fp_as);
    fclose(fp_am);

    return 0;
}



int preprocessor_full_flow(char *file_name){

    FILE  *first_copy;
    node *head;
    char *am_file_name, clean_file_name[MAX_LINE_LENGTH];
    int indication, line_counter = 0;

    head = NULL;
    indication = 0;
    if(check_as_file_ending(file_name)!=0){
        error_log(file_name,line_counter,ARG_NOT_AS_FILE);
        return -1;
    }

    snprintf(clean_file_name, sizeof(clean_file_name), "%s_clean.as", file_name);

    first_copy = create_clean_file(file_name, clean_file_name);
    if (first_copy == NULL) {
        printf(FAIL_CLEAN_FILE);
        return -1;
    }

    indication = prepro_first_pass(file_name,clean_file_name,&line_counter, &head);
    if (indication){
        /*error_log(file_name,line_counter, FAIL_EXTRACT_MACROS);*/
        /* release mcro linked list, delete files */
        free_linked_list(head);
        remove(clean_file_name);
        return -1;
    }

    am_file_name = change_ending_of_file(file_name, ".am");
    if (am_file_name == NULL) {
        /* release mcro linked list, delete files */
        free_linked_list(head);
        remove(clean_file_name);
        return -1;
    }

    indication = preproc_second_pass(file_name,&head,clean_file_name,&line_counter,am_file_name);

    if (indication){
        /*error_log(file_name,line_counter,FAIL_TO_SWITCH_MCRO_NAME);*/
        /* release mcro linked list, delete files */
        free_linked_list(head);
        free(am_file_name);
        remove(clean_file_name);
        return -1;
    }

    /* release mcro linked list, delete and close files */
    free_linked_list(head);
    free(am_file_name);
    remove(clean_file_name);

    printf(PREPROCCESSOR_SUCCESS);
    return 0;
}