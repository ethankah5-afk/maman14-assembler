/* 
* Created by Ethan and Yakir 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "first_pass.h"
#include "parser.h"
#include "sec_pass.h"
#include "errors.h"
#include "tables.h"
#include "constants.h"
#include "encoder.h"

/*
* Handle .entry line 
* line - source line 
* line_num - current line number 
* file_name - source file name 
* entries - entries table 
* return - status of the code 
*/

int handle_entry_line(char *line, int line_num, char*file_name,  NameRefTable *entries) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            print_external_error(ERROR_20, loc);
            return 0;
        }
    }
    if (strcmp(token, ".entry") != 0) {
        return 0;
    }
    token = strtok(NULL, " \t\n");

    if (token == NULL) {
        print_external_error(ERROR_20, loc);
        return 0;
    }
    /* Validate entry label */
    if(!is_label_operands(token)) {
        print_external_error(ERROR_23, loc);
        return 0;
    }
    /* Extra text after label */
    if (strtok(NULL, " \t\n")!=NULL) {
        print_external_error(ERROR_18, loc);
        return 0;
    }
  /* Save entry label */
    if (!add_name_ref(entries, token, line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    return 1;
}

/*
* Handle .data line 
* line - soruce line 
* line_num - current line number
* file_name - source file name 
* labels - labels table 
* data_img - data image 
* DC - data counter 
* table - labels table 
* marco_table - macros table
* macro_count - number of macros
* return - status of the code 
*/
int handle_data_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count) {
    char temp[MAX_LINE_LENGTH];
    char label_name[31];
    char *token;
    char *operands;
    int status;
    int has_label;
    ERROR_CODES error_code;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    strcpy(temp, line);
    has_label = 0;
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    /* Check if the line starts with a label */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,table,macro_table,macro_count)) {
            print_external_error(ERROR_28, loc);
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            print_external_error(ERROR_30, loc);
            return 0;
        }
    }
    if (strcmp(token, ".data") != 0) {
        print_external_error(ERROR_38, loc);
        return 0;
    } 
    
    /* Add label before data values */
    if (has_label) {

        if (findLabel(labels, label_name) != -1) {
            print_external_error(ERROR_35, loc);
            return 0;
        }
        if (!addLabel(labels, label_name, *DC, 1, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
    }
    operands=strstr(line,".data");
    if (operands == NULL) {
        print_external_error(ERROR_38,loc);
        return 0;
    }
    /* Move after .data */
    operands+=5;
    status=parse_data_values(operands,data_img,line_num,DC,&error_code);
    if (status==-1) {
        print_internal_error(error_code);
        return -1;
    }
    if (status==0) {
        print_external_error(error_code,loc);
        return 0;
    }
    return 1;
}


/*
* Handle .string line
* line - source line 
* line_num - current line number 
* file_name - source file name 
* labels - labels table 
* data_img - data image 
* DC  - data counter 
* table - labels table 
* macro_table - macros table 
* marco_count - number of macros 
* return - status code 
*/
int handle_string_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count) {
    char temp[MAX_LINE_LENGTH];
    char label_name[31];
    char *token;
    char *start_quote;
    char *end_quote;
    int has_label;
    int i;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    strcpy(temp, line);
    has_label = 0;
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    /* check if line starts with a label */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,table,macro_table,macro_count)) {
             print_external_error(ERROR_28, loc);
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            print_external_error(ERROR_30, loc);
            return 0;
        }
    }

    if (strcmp(token, ".string") != 0) {
        print_external_error(ERROR_38, loc);
        return 0;
    }
    /* add label to table if found */
    if (has_label) {
        if (findLabel(labels, label_name) != -1) {
    print_external_error(ERROR_35, loc);
    return 0;
}

    if (!addLabel(labels, label_name, *DC, 1, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
    }
    /* Find opening quote */
    start_quote = strchr(line, '"');
    if (start_quote == NULL) {
           print_external_error(ERROR_33, loc);
        return 0;
    }
     /* Find closing quote */
    end_quote = strrchr(line, '"');
    if (end_quote == NULL || end_quote == start_quote) {
        print_external_error(ERROR_33, loc);
        return 0;
    }
    /* Validate extra text after string */
    {
        char *after_quote = end_quote + 1;
        while (*after_quote == ' ' || *after_quote == '\t' || *after_quote == '\n') {
            after_quote++;
        }
        if (*after_quote != '\0') {
              print_external_error(ERROR_34, loc);
            return 0;
        }
    }
    /* Move after opeing quote */
    start_quote++;

    /* Encoding string characters */
    for (i = 0; start_quote + i < end_quote; i++) {
        if (!add_code_word(data_img, (unsigned short)start_quote[i], NULL, line_num)) {
             print_internal_error(ERROR_1);
            return -1;
        }
        (*DC)++;
    }
    if (!add_code_word(data_img, (unsigned short)'\0', NULL, line_num)) {
         print_internal_error(ERROR_1);
        return -1;
    }
    (*DC)++;
    return 1;
}

/* 
* Initialize first pass memory structures
* labels - labels table 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* entries - entries table 
* return - 1 if success if not 0 
*/
int init_first_pass_memory(LabelTable *labels,
                           CodeImage *code_img,
                           CodeImage *data_img,
                           NameRefTable *externs,
                           NameRefTable *entries) {
    
    if (!initLabelTable(labels)) {
        return 0;
    }

    if (!init_code_image(code_img)) {
        free_label_table(labels);
        return 0;
    }

    if (!init_code_image(data_img)) {
        free_label_table(labels);
        free_code_image(code_img);
        return 0;
    }
    
  /* Initialize  extern labels table */
    if (!init_name_ref_table(externs)) {
        free_label_table(labels);
        free_code_image(code_img);
        free_code_image(data_img);
        return 0;
    }
    
    /* Initialize entries table */
    if (!init_name_ref_table(entries)) {
        free_label_table(labels);
        free_code_image(code_img);
        free_code_image(data_img);
        free_name_ref_table(externs);
        return 0;
    }

    return 1;
}
/* 
* Free first pass memory structures
* labels - labels table 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* entries - entries table 
*/
void free_first_pass_memory(LabelTable *labels,
                            CodeImage *code_img,
                            CodeImage *data_img,
                            NameRefTable *externs,
                            NameRefTable *entries) {
    free_label_table(labels);
    free_code_image(code_img);
    free_code_image(data_img);
    free_name_ref_table(externs);
    free_name_ref_table(entries);
}


/* 
* Update data labels address after first pass
* labels - labels table 
* IC - final instruction counter
*/
void update_data_labels(LabelTable *labels, int IC) {
    int i;
    for (i=0; i<labels->count;i++) {
        /* updaate only data labels */
        if (labels->arr[i].is_data) {
            labels->arr[i].address+=IC;
        }
    }
}
/* 
* Handle .extern 
* line - source line
* line_num - current line number 
* file_name - source file name 
* labels - labels table 
* externs - extern label name 
* label_name - extern label name 
* macro_table - macros table 
* macro_count - number of macros 
* return - status of the code
*/
int handle_extern_line(char *line, int line_num,char *file_name, LabelTable *labels, NameRefTable *externs, char *label_name,macro_node *macro_table,int macro_count) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    strcpy(temp,line);
    token = strtok(temp," \t\n");
    if (token == NULL) {
        return 0; }
    
    /* Labels before .extern are not legal */
    if (token[strlen(token) -1] == ':') {
        print_external_error(ERROR_28, loc);
        return 0; }
    if (strcmp(token, ".extern") !=0) {
        return 0; }
    token = strtok(NULL, " \t\n");

    if (token ==NULL) {
        print_external_error(ERROR_20, loc);
        return 0;
    }
    /* Validate extern label */
    if(!is_valid_label(token,labels,macro_table,macro_count)) {
        print_external_error(ERROR_28, loc);
        return 0; }

    strcpy(label_name, token);
    token = strtok(NULL, " \t\n");
    
    /* Extra text after extern label */
    if (token!=NULL) {
        print_external_error(ERROR_18, loc);
        return 0; }
    
    /* Prevent duplicate label definitions */
    if (findLabel(labels, label_name) != -1) {
        print_external_error(ERROR_36, loc);
        return 0;
    }
    if (!addLabel(labels, label_name, 0,0,line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    
    /* Mark label as external */
    labels->arr[labels->count - 1].is_extern = 1;
    /* Save extern reference */
    if (!add_name_ref(externs, label_name, line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    return 1;
}

/* 
* Handle line during first pass
* line - source line 
* line_num - current line number 
* file_name - source file name 
* labels -labels table 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* entries - entries table 
* IC - instruction counter 
* DC - data counter 
* macro_table - macros table 
* macro_count - number of macros 
* return - status of the code
*/

int handle_first_pass_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int *IC,int *DC,macro_node *macro_table,int macro_count) {
    int line_type;
    char extern_label[31];
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    
      /* Decide which handler should process the line */
    line_type=detect_line_type(line);
    switch(line_type) {
        case LINE_EMPTY:
            return 1;
        case LINE_ENTRY:
            return handle_entry_line(line,line_num,file_name,entries);
        case LINE_EXTERN:
            return handle_extern_line(line,line_num,file_name,labels,externs,extern_label,macro_table,macro_count);
        case LINE_DATA:
            return handle_data_line(line,line_num,file_name,labels,data_img,DC,labels,macro_table,macro_count);
        case LINE_STRING:
            return handle_string_line(line,line_num,file_name,labels,data_img,DC,labels,macro_table,macro_count);
        case LINE_INSTRUCTION:
            return handle_instruction_line(line,line_num,file_name,labels,code_img,IC,macro_table,macro_count);
        case LINE_ERROR:
        default:
            print_external_error(ERROR_17,loc);
            return 0;
    }
}
/* 
* Execute first and second assembler passes
* file_name - source file name 
* macro_table - macros table 
* macro_count - number of macros 
* return - 1 if assembler passes succeeded otherwise 0 
*/
int exe_passes(char *file_name,macro_node *macro_table,int macro_count){
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_num;
    int IC, DC;
    int error_found;
    LabelTable labels;
    CodeImage code_img;
    CodeImage data_img;
    NameRefTable externs;
    NameRefTable entries;
    location loc;
    int status;
    line_num = 0;
    IC = 100;
    DC = 0;
    error_found = 0;
    loc.file_name = file_name;
    
    /* Prepare all tabels and code images for first pass */
    if (!init_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        print_internal_error(ERROR_8);
        free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
        return 0;
    }
    /* Read source file line by line  */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_num++;
        loc.line_num = line_num;
        /* Check if line is longer then allowed */
        if (strlen(line) == MAX_LINE_LENGTH - 1 && line[MAX_LINE_LENGTH - 2] != '\n') {
            print_external_error(ERROR_6, loc);
            error_found = 1;

            while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
                if (strchr(line, '\n') != NULL) {
                    break;
                }
            }

            continue;
        }
        /* Skip empty lines or comments */
        if (is_blank_or_comment(line)) {
            continue;
        }
        status=handle_first_pass_line(line, line_num,file_name, &labels, &code_img, &data_img,&externs, &entries, &IC, &DC,macro_table,macro_count);
            if (status==-1) {
                fclose(fp);
                free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
                return 0;
            }
            if (status==0) {
                error_found = 1;
            }

    }
    fclose(fp);
    
    /* Data labels get final addresses only after IC is known */
    update_data_labels(&labels,IC);
    if (!error_found) {
        if (!exe_sec_pass(file_name,&labels,&code_img,&data_img,&externs,&entries,IC,DC)) {
            printf("Second pass failed\n");
            error_found = 1;
        }
    }
    else {
        printf("first pass failed\n");
    }
    free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
    return !error_found;
}
