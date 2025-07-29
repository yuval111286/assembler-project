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

int prepro_first_pass(char *file_name, node **head){

    FILE *fp;
    fpos_t pos;
    char line[MAX_LINE_LENGTH],*mcro_name = NULL, *mcro_content = NULL;
    int line_counter, starting_mcro;

    fp = fopen(file_name,"r");
    line_counter = 0;

    if (fp == NULL) {
        printf(FILE_NOT_OPEN_READING);
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_counter++;

        /* first word in the line is mcro*/
        if (strncmp(line, MCRO, 5) == 0){
            starting_mcro = line_counter;
            mcro_name = identify_macro_name(line);
            if (mcro_name == NULL) {
                fclose(fp);
                return -1;
            }
            fgetpos(fp, &pos);
            mcro_content = extract_mcro_text(fp,&pos,&line_counter);
            if (mcro_content == NULL) {
                free(mcro_name);
                fclose(fp);
                return -1;
            }
            fsetpos(fp, &pos);
            add_node_to_linked_list(head,mcro_name,mcro_content,starting_mcro);

        }
    }

    fclose(fp);
    return 0;
}

char *identify_macro_name(char *line){

    char *mcro_name, *extra_text_after_mcro_name;

    mcro_name = skip_word(line);
    if(mcro_name == NULL)
    {
        printf(MACRO_WITHOUT_NAME);
        return NULL;
    }
    extra_text_after_mcro_name= skip_word(mcro_name);
    if(extra_text_after_mcro_name !=NULL)
    {
        printf(EXTRA_TEXT_AFTER_MCRO_START);
        return NULL;
    }

    if (!mcro_name_validation(mcro_name))
    {
        printf(ILLEGAL_MACRO_NAME);
        return NULL;
    }

   return mcro_name;

}

int mcro_name_validation(char *mcro_name){

    return (identify_opcode(mcro_name)&& identify_register(mcro_name)&& is_directive(mcro_name));

}

char *extract_mcro_text(FILE *fp, fpos_t *pos, int *line_counter) {
    char str[MAX_LINE_LENGTH]={0};
    char *mcro, *extra;
    int mcro_length = 0;


    /* return to first position */
    if (fsetpos(fp, pos) != 0) {
        internal_error_log(FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }

    while (fgets(str, MAX_LINE_LENGTH, fp)) {
        (*line_counter)++;

        if (strncmp(str, MCROEND, 7) == 0) {
            /* no extra text after mcroend */
            extra = skip_word(MCROEND);
            if (extra && *extra != '\0' && *extra != '\n') {
                internal_error_log(EXTRA_TEXT_AFTER_MCROEND);
                return NULL;
            }
            break; /* reach to mcroend*/
        }

        mcro_length += strlen(str);
    }

    /* return to first position */
  /*  if (fsetpos(fp, pos) != 0) {
        internal_error_log(FAIL_TO_SET_POSITION_IN_FILE);
        return NULL;
    }
*/
    mcro = copy_text_from_file_to_string(fp, pos, mcro_length);

    return mcro;
}


int preproc_second_pass(node **head,char *as_file_name, char *am_file_name){

    FILE *fp_as, *fp_am;
    char line[MAX_LINE_LENGTH];
    int found;
    node *macro_node;


    fp_as = fopen(as_file_name,"r");
    if (fp_as == NULL) {
        printf(FILE_NOT_OPEN_READING);
        return -1;
    }

    fp_am = fopen(am_file_name,"w");
    if (fp_am == NULL) {
        printf(FILE_NOT_OPEN_WRITING);
        return -1;
    }

    while (fgets(line, sizeof(line), fp_as) != NULL) {
        if (strncmp(line, MCRO, 5) == 0) {
            /* skip lines until mcroend */
            while (fgets(line, sizeof(line), fp_as) != NULL) {
                if (strncmp(line, MCROEND, 8) == 0) {
                    break;
                }
            }
        }

        macro_node = search_node_in_linked_list(head, line, &found);

        if (found) {
            /*mcro name will be switched with definition */
            fputs(macro_node->text, fp_am);
        } else {
             /* regular line will be copied to am file */
            fputs(line, fp_am);
        }


    }
    return 0;

}



int preprocessor_full_flow(char *file_name){

    FILE /* *fp_read, *fp_write,*/ *first_copy;
    node *head;
    /*node *head = NULL;*/
    char *am_file_name, clean_file_name[256];
    /*SourceFileLocation as_file;*/
    int indication;

    head = NULL;
    indication = 0;
    if(check_as_file_ending(file_name)!=0){
        printf(ARG_NOT_AS_FILE);
        return -1;
    }

    snprintf(clean_file_name, sizeof(clean_file_name), "%s_clean.as", file_name);

    first_copy = create_clean_file(file_name, clean_file_name);
    if (first_copy == NULL) {
        printf(FAIL_CLEAN_FILE);
        return -1;
    }

    indication = prepro_first_pass(clean_file_name,&head);
    if (indication){
        printf(FAIL_EXTRACT_MACROS);
        return -1;
    }

    am_file_name = change_ending_of_file(file_name, ".am");


    indication = preproc_second_pass(&head,file_name ,am_file_name);
    if (indication){
        printf(FAIL_TO_SWITCH_MCRO_NAME);
        return -1;
    }

 return 0;
}