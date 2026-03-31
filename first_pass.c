//
// Created by ethan on 15/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"

#include <stdlib.h>

#include "main_struct.h"
#define MAX_LINE_LENGTH 80
int is_label(char *line_of_file,char *label_name) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp,line_of_file);
    token= strtok(temp," \t\n");
    if (token!=NULL&&token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1]='\0';
        if (!is_valid_label(token)) {
            return 0;
        }
        strcpy(label_name,token);
        return 1;
    }
    return 0;
}

int is_data(char *line) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) return 0;
    /* skip label if exists */
    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) return 0;
    }
    return strcmp(token, ".data") == 0;
}
int is_string(char *line) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) return 0;
    /* skip label if exists */
    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) return 0;
    }
    return strcmp(token, ".string") == 0;
}
int is_entry(char *line) {
    char temp[MAX_LINE_LENGTH];
    char *token;

    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) return 0;

    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) return 0;
    }

    if (strcmp(token, ".entry") != 0) return 0;

    token = strtok(NULL, " \t\n");
    if (token == NULL || !is_valid_label(token)) return 0;

    token = strtok(NULL, " \t\n");
    if (token != NULL) return 0;

    return 1;
}
int is_extern(char *line, char *label_name) {
    char temp[MAX_LINE_LENGTH];
    char *token;

    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) return 0;

    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) return 0;
    }

    if (strcmp(token, ".extern") != 0) return 0;

    token = strtok(NULL, " \t\n");
    if (token == NULL || !is_valid_label(token)) return 0;

    strcpy(label_name, token);

    token = strtok(NULL, " \t\n");
    if (token != NULL) return 0;

    return 1;
}
int initLabelTable(LabelTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (Label *)malloc(table->capacity * sizeof(Label));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}

int findLabel(LabelTable *table, char *name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->arr[i].symbol_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined) {
    Label *temp;
    /* בדיקה אם כבר קיים */
    if (findLabel(table, name) != -1) {
        return 0;
    }
    /* הגדלת מערך אם צריך */
    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (Label *)realloc(table->arr, table->capacity * sizeof(Label));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    }
    /* הכנסת הנתונים */
    strcpy(table->arr[table->count].symbol_name, name);
    table->arr[table->count].address = address;
    table->arr[table->count].line_defined = line_defined;
    table->arr[table->count].is_data = is_data;
    table->arr[table->count].is_entry = 0;
    table->arr[table->count].is_extern = 0;
    table->count++;
    return 1;
}

int add_code_word(CodeImage *img, unsigned short value, const char *label, int line) {
    CodeWord *temp;

    if (img->count == img->capacity) {
        img->capacity *= 2;
        temp = (CodeWord *)realloc(img->arr, img->capacity * sizeof(CodeWord));
        if (temp == NULL) {
            return 0;
        }
        img->arr = temp;
    }

    img->arr[img->count].value = value;
    img->arr[img->count].assembly_line = line;

    if (label != NULL) {
        img->arr[img->count].label = (char *)malloc(strlen(label) + 1);
        if (img->arr[img->count].label == NULL) {
            return 0;
        }
        strcpy(img->arr[img->count].label, label);
    } else {
        img->arr[img->count].label = NULL;
    }

    img->count++;
    return 1;
}

void free_code_image(CodeImage *img) {
    int i;

    for (i = 0; i < img->count; i++) {
        if (img->arr[i].label != NULL) {
            free(img->arr[i].label);
        }
    }

    free(img->arr);
    img->arr = NULL;
    img->count = 0;
    img->capacity = 0;
}

int init_code_image(CodeImage *img) {
    img->count = 0;
    img->capacity = 10;
    img->arr = (CodeWord *)malloc(img->capacity * sizeof(CodeWord));
    if (img->arr == NULL) {
        return 0;
    }
    return 1;
}

int  init_name_ref_table(NameRefTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (NameRef *)malloc(table->capacity * sizeof(NameRef));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}

void free_name_ref_table(NameRefTable *table) {
    int i;
    for (i = 0; i < table->count; i++) {
        free(table->arr[i].name);
    }
    free(table->arr);
    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}

int add_name_ref(NameRefTable *table, const char *name, int line) {
    NameRef *temp;

    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (NameRef *)realloc(table->arr, table->capacity * sizeof(NameRef));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    }
    table->arr[table->count].name = (char *)malloc(strlen(name) + 1);
    if (table->arr[table->count].name == NULL) {
        return 0;
    }
    strcpy(table->arr[table->count].name, name);
    table->arr[table->count].assembly_line = line;
    table->count++;
    return 1;
}

void free_label_table(LabelTable *table) {
    if (table->arr != NULL) {
        free(table->arr);
    }

    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}
int handle_entry_line(char *line, int line_num, NameRefTable *entries) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp, line);
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    if (token[strlen(token) - 1] == ':') {
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            return 0;
        }
    }
    if (strcmp(token, ".entry") != 0) {
        return 0;
    }
    token = strtok(NULL, " \t\n");
    if (token == NULL || !is_valid_label(token)) {
        return 0;
    }
    if (!add_name_ref(entries, token, line_num)) {
        return 0;
    }
    return 1;
}
int handle_data_line(char *line, int line_num, LabelTable *labels, CodeImage *data_img, int *DC) {
    char temp[MAX_LINE_LENGTH];
    char label_name[31];
    char *token;
    int has_label;
    int num;
    strcpy(temp, line);
    has_label = 0;
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    /* אם יש label בתחילת שורה */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token)) {
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            return 0;
        }
    }
    /* חייב להיות .data */
    if (strcmp(token, ".data") != 0) {
        return 0;
    }
    /* אם יש label, מכניסים אותו לטבלת התוויות */
    if (has_label) {
        if (!addLabel(labels, label_name, *DC, 1, line_num)) {
            return 0;
        }
    }
    /* עכשיו מתחילים לקרוא את המספרים */
    token = strtok(NULL, ", \t\n");
    if (token == NULL) {
        return 0;
    }
    while (token != NULL) {
        num = atoi(token);
        if (!add_code_word(data_img, (unsigned short)num, NULL, line_num)) {
            return 0;
        }
        (*DC)++;
        token = strtok(NULL, ", \t\n");
    }
    return 1;
}

int is_valid_lable(char *label) {
    int i;
    if (strlen(label)>31||label==NULL)return 0;
    if (!isalpha((unsigned char)label[0]))return 0;
    if ((unsigned char)label[strlen(label)-1]!=':')return 0;
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum((unsigned char)label[i])) {
            return 0;
        }
    }
    //if label name is taken
    return 1;
}
int handle_string_line(char *line, int line_num, LabelTable *labels, CodeImage *data_img, int *DC) {
    char temp[MAX_LINE_LENGTH];
    char label_name[31];
    char *token;
    char *start_quote;
    char *end_quote;
    int has_label;
    int i;
    strcpy(temp, line);
    has_label = 0;
    token = strtok(temp, " \t\n");
    if (token == NULL) {
        return 0;
    }
    /* אם יש label בתחילת השורה */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token)) {
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            return 0;
        }
    }
    /* חייב להיות .string */
    if (strcmp(token, ".string") != 0) {
        return 0;
    }
    /* אם יש label, מכניסים אותו */
    if (has_label) {
        if (!addLabel(labels, label_name, *DC, 1, line_num)) {
            return 0;
        }
    }
    start_quote = strchr(line, '"');
    if (start_quote == NULL) {
        return 0;
    }
    end_quote = strrchr(line, '"');
    if (end_quote == NULL || end_quote == start_quote) {
        return 0;
    }
    start_quote++;
    for (i = 0; start_quote + i < end_quote; i++) {
        if (!add_code_word(data_img, (unsigned short)start_quote[i], NULL, line_num)) {
            return 0;
        }
        (*DC)++;
    }
    if (!add_code_word(data_img, (unsigned short)'\0', NULL, line_num)) {
        return 0;
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
int handle_first_pass_line(char *line,
                           int line_num,
                           LabelTable *labels,
                           CodeImage *code_img,
                           CodeImage *data_img,
                           NameRefTable *externs,
                           NameRefTable *entries,
                           int *IC,
                           int *DC) {
    char extern_label[31];
    if (is_blank_or_comment(line)) {
        return 1;
    }
    if (is_entry(line)) {
        return handle_entry_line(line, line_num, entries);
    }
    if (is_extern(line, extern_label)) {
        return add_name_ref(externs, extern_label, line_num);
    }
    if (is_data(line)) {
        return handle_data_line(line, line_num, labels, data_img, DC);
    }
    if (is_string(line)) {
        return handle_string_line(line, line_num, labels, data_img, DC);
    }
    return handle_instruction_line(line, line_num, labels, code_img, IC);
}
//int is_blank_or_comment(char *line);
//int is_valid_label(char *line);
int exe_first_pass(char *file_name) {
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

    line_num = 0;
    IC = 1;
    DC = 0;
    error_found = 0;

    if (!init_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries)) {
        return 0;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_num++;
        if (!handle_first_pass_line(line, line_num, &labels, &code_img, &data_img,
                                    &externs, &entries, &IC, &DC)) {
            error_found = 1;
                                    }
    }
    fclose(fp);
    free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);

    return !error_found;
}


/*int exe_first_pass(char *file_name) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int line_num;
    int IC;
    int DC;
    int error_found;
    int label_found;

    LabelTable labels;
    CodeImage code_img;
    CodeImage data_img;
    NameRefTable externs;
    NameRefTable entries;

    line_num = 0;
    IC = 0;
    DC = 0;
    error_found = 0;
    label_found=0;

    if (!initLabelTable(&labels)) {
        return 0;
    }

    if (!init_code_image(&code_img)) {
        free_label_table(&labels);
        return 0;
    }

    if (!init_code_image(&data_img)) {
        free_label_table(&labels);
        free_code_image(&code_img);
        return 0;
    }

    if (!init_name_ref_table(&externs)) {
        free_label_table(&labels);
        free_code_image(&code_img);
        free_code_image(&data_img);
        return 0;
    }

    if (!init_name_ref_table(&entries)) {
        free_label_table(&labels);
        free_code_image(&code_img);
        free_code_image(&data_img);
        free_name_ref_table(&externs);
        return 0;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        free_label_table(&labels);
        free_code_image(&code_img);
        free_code_image(&data_img);
        free_name_ref_table(&externs);
        free_name_ref_table(&entries);
        return 0;
    }
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_num++;
        if (is_blank_or_comment(line)) {
            continue;
        }
        char extern_label[31];

        if (is_entry(line)) {
            if (!handle_entry_line(line,line_num,&entries)){
                error_found=1;
            }
            continue;
        }
        if (is_extern(line, extern_label)) {
            if (!add_name_ref(&externs, extern_label, line_num)) {
                error_found = 1;
            }
            continue;
        }
        if (is_data(line)) {
            if (!handle_data_line(line,line_num,&labels,&data_img,&DC)){
                error_found = 1;
            }
            continue;
        }
        if(is_string(line)) {
            if (!handle_string_line(line,line_num,&labels,&data_img,&DC)) {
                error_found=1;
            }
            continue;
        }
        convert_instruction(line);
    }
    fclose(fp);
    free_label_table(&labels);
    free_code_image(&code_img);
    free_code_image(&data_img);
    free_name_ref_table(&externs);
    free_name_ref_table(&entries);

    /*    LabelTable labels;
    CodeImage code_img;
    CodeImage data_img;
    NameRefTable externs;
    NameRefTable entries;*/
//    return 1;
//}

/*int run_first_pass(char *file_name){
    FILE *fp_in;
    int line_b
    int IC=0;
    int DC=0;
    fp_in = fopen(file_name, "r");
    LabelTable label_table;
    initLabelTable(&label_table);
    if (fp_in == NULL) return 0;
    char line[MAX_LINE];
    char label_name[MAX_LINE];
    int found=0;
    while (fgets(line, MAX_LINE, fp_in)!=NULL) {
        found=is_label(line,label_name);
        int data   = is_data(line);
        int string = is_string(line);
        if (data||string) {
            if (found) {
                addLabel(&label_table,label_name,"data",DC,1);
            }
            if (data) {
                int lines_data=handel_data(line);
                DC+=lines_data;
            }
            else {
                int lines_string=handel_string(line);
                DC+=lines_string;
                DC++;
            }
        }
        else if(is_entry(line)||is_extern(line,label_name)) {
            if (is_entry(line)) {
                continue;
            }
            else {//is extern
                addLabel(&label_table,label_name,"external",0,0);
            }
        }
        else{
            //instruction- or error
            if (found) {
                addLabel(&label_table,label_name,"code",IC,0);
            }
            convert_instruction(line);
        }
    }
    fclose(fp_in);
    return 1;
}

*/
