//
// Created by ethan on 15/03/2026.
//
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

    if(!is_label_operands(token)) {
        print_external_error(ERROR_23, loc);
        return 0;
    }
    if (strtok(NULL, " \t\n")!=NULL) {
        print_external_error(ERROR_18, loc);
        return 0;
    }
    if (!add_name_ref(entries, token, line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    return 1;
}
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
    /* אם יש label בתחילת השורה */
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
    /* חייב להיות .string */
    if (strcmp(token, ".string") != 0) {
        print_external_error(ERROR_38, loc);
        return 0;
    }
    /* אם יש label, מכניסים אותו */
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
    start_quote = strchr(line, '"');
    if (start_quote == NULL) {
           print_external_error(ERROR_33, loc);
        return 0;
    }

    end_quote = strrchr(line, '"');
    if (end_quote == NULL || end_quote == start_quote) {
        print_external_error(ERROR_33, loc);
        return 0;
    }
    /* בדיקת extra text אחרי המחרוזת */
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
    start_quote++;
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

    if (!init_name_ref_table(externs)) {
        free_label_table(labels);
        free_code_image(code_img);
        free_code_image(data_img);
        return 0;
    }

    if (!init_name_ref_table(entries)) {
        free_label_table(labels);
        free_code_image(code_img);
        free_code_image(data_img);
        free_name_ref_table(externs);
        return 0;
    }

    return 1;
}
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



void update_data_labels(LabelTable *labels, int IC) {
    int i;
    for (i=0; i<labels->count;i++) {
        if (labels->arr[i].is_data) {
            labels->arr[i].address+=IC;
        }
    }
}

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

    if(!is_valid_label(token,labels,macro_table,macro_count)) {
        print_external_error(ERROR_28, loc);
        return 0; }

    strcpy(label_name, token);
    token = strtok(NULL, " \t\n");

    if (token!=NULL) {
        print_external_error(ERROR_18, loc);
        return 0; }

    if (findLabel(labels, label_name) != -1) {
        print_external_error(ERROR_36, loc);
        return 0;
    }
    if (!addLabel(labels, label_name, 0,0,line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    labels->arr[labels->count - 1].is_extern = 1;
    if (!add_name_ref(externs, label_name, line_num)) {
        print_internal_error(ERROR_1);
        return -1;
    }
    return 1;
}

int handle_first_pass_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int *IC,int *DC,macro_node *macro_table,int macro_count) {
    int line_type;
    char extern_label[31];
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
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
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_num++;
        loc.line_num = line_num;
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
