//
// Created by ethan on 15/03/2026.
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include <stdlib.h>
#include "main_struct.h"
#include "sec_pass.h"
#include "pre_proc.h"
#define ADDR_INVALID  (-1)
#define ADDR_IMMEDIATE 0
#define ADDR_DIRECT    1
#define ADDR_RELATIVE  2
#define ADDR_REGISTER  3
#define LINE_ERROR -1
#define LINE_EMPTY 0
#define LINE_ENTRY 1
#define LINE_EXTERN 2
#define LINE_DATA 3
#define LINE_STRING 4
#define LINE_INSTRUCTION 5

int is_blank_or_comment(const char *line) {
    int i=0;
    while (line[i]==' '||line[i]=='\t') {
        i++;
    }
    if (line[i]=='\n'||line[i]=='\0') {
        return 1;
    }
    if (line[i]==';') {
        return 1;
    }
    return 0;
}
int is_valid_label(char *label_name, LabelTable *table,macro_node *macro_table,int macro_count){
    int i;
    if (label_name == NULL) {
        return 0;
    }
    if (label_name[0] == '\0') {
        return 0;
    }
    if (strlen(label_name) > 30) {
        return 0;
    }
    if (!isalpha((unsigned char)label_name[0])) {
        return 0;
    }
    for (i = 1; label_name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)label_name[i])) {
            return 0;
        }
    }
    /* if label name is taken */
    if (table != NULL && findLabel(table, label_name) != -1) {
        return 0;
    }
    if (find_macro(macro_table, macro_count, label_name) != -1) {
        return 0;
    }
    if (findInstruction(label_name) != NULL) {
        return 0;
    }
    if (findReg(label_name)!= -1) {
        return  0;
    }
    if (strcmp(label_name,".data")==0|| strcmp(label_name,".string")==0 || strcmp(label_name, ".extern")==0 || strcmp(label_name,".entry")==0) {
        return 0;
    }
    return 1;
}
int is_label_operands(char *label_name){
    int i;
    if (label_name == NULL) {
        return 0;
    }
    if (label_name[0] == '\0') {
        return 0;
    }
    if (strlen(label_name) > 30) {
        return 0;
    }
    if (!isalpha((unsigned char)label_name[0])) {
        return 0;
    }
    for (i = 1; label_name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)label_name[i])) {
            return 0;
        }
    }
    if (findInstruction(label_name) != NULL) {
        return 0;
    }
    if (findReg(label_name)!= -1) {
        return  0;
    }
    if (strcmp(label_name,".data")==0|| strcmp(label_name,".string")==0 || strcmp(label_name, ".extern")==0 || strcmp(label_name,".entry")==0) {
        return 0;
    }
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

int findLabel(LabelTable *table,char *name) {
    int i;
    if (table==NULL||name==NULL) {
        return -1;
    }
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->arr[i].symbol_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined) {
    Label *temp;
    if (findLabel(table, name) != -1) {
        return 0;
    }
    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (Label *)realloc(table->arr, table->capacity * sizeof(Label));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    }
    if (strlen(name)>30) {
        return 0;
    }
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
            img->arr[img->count].value = 0;
            img->arr[img->count].assembly_line = 0;
            return 0;
        }
        strcpy(img->arr[img->count].label, label);
    }
    else {
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
    if (token == NULL || !is_label_operands(token)) {
        return 0;
    }
    if (strtok(NULL, " \t\n")!=NULL) {
        return 0;
    }
    if (!add_name_ref(entries, token, line_num)) {
        return 0;
    }
    return 1;
}
int handle_data_line(char *line, int line_num, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count) {
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
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,table,macro_table,macro_count)) {
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            return 0;
        }
    }
    if (strcmp(token, ".data") != 0) {
        return 0;
    }
    if (has_label) {
        if (!addLabel(labels, label_name, *DC, 1, line_num)) {
            return 0;
        }
    }
    token = strtok(NULL, ", \t\n");
    if (token == NULL) {
        return 0;
    }
    while (token != NULL) {
        if (!is_valid_number(token,&num)){
            return 0;
        }
        if (!add_code_word(data_img, (unsigned short)num, NULL, line_num)) {
            return 0;
        }
        (*DC)++;
        token = strtok(NULL, ", \t\n");
    }
    return 1;
}

int is_valid_number(const char *token, int *value) {
    char *endptr;
    long num;
    if (token == NULL || *token == '\0') {
        return 0;
    }
    num = strtol(token, &endptr, 10);
    if (*endptr != '\0') {
        return 0;
    }
    if (num<-2048||num>2047){
        return 0;
    }
    *value= (int)num;
    return 1;
}
int handle_string_line(char *line, int line_num, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count) {
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
        if (!is_valid_label(token,table,macro_table,macro_count)) {
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
    /* בדיקת extra text אחרי המחרוזת */
    {
        char *after_quote = end_quote + 1;
        while (*after_quote == ' ' || *after_quote == '\t' || *after_quote == '\n') {
            after_quote++;
        }
        if (*after_quote != '\0') {
            return 0;
        }
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
int get_addressing_type(char *op) {
    if (op == NULL || op[0] == '\0') {
        return ADDR_INVALID;
    }
    if (op[0] == '#') {
        int num;
        if (!is_valid_number(op+1,&num)) {
            return ADDR_INVALID;
        }
        return ADDR_IMMEDIATE;
    }
    if (op[0] == '%') {
        if (is_label_operands(op + 1)) {
            return ADDR_RELATIVE;
        }
        return ADDR_INVALID;
    }
    if (findReg(op) != -1) {
        return ADDR_REGISTER;
    }
    if (is_label_operands(op)) {
        return ADDR_DIRECT;
    }
    return ADDR_INVALID;
}
void parse_operands(char *operands_line, char *op1, char *op2, int *count) {
    char temp [MAX_LINE_LENGTH];
    char *token;
    *count=0;
    op1[0]='\0';
    op2[0]='\0';
    if (operands_line==NULL) {
        return;
    }
    strcpy(temp,operands_line);
    token=strtok(temp,",");
    if (token==NULL) {
        return;
    }
    strcpy(op1,token);
    trim_spaces(op1);
    (*count)++;
    token=strtok(NULL,",");
    if (token==NULL) {
        return;
    }
    strcpy(op2,token);
    trim_spaces(op2);
    (*count)++;
    token = strtok(NULL, ",");
    if (token != NULL) {
        *count = -1; /* למשל סימון שגיאה */
    }
}
unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count) {
    unsigned short word = 0;
    int src_type=0;
    int dest_type=0;
    if (op_count==2) {
        src_type=get_addressing_type(op1);
        dest_type=get_addressing_type(op2);
    }
    else if (op_count==1) {
        src_type=0;
        dest_type=get_addressing_type(op1);
    }
    else {
        src_type=0;
        dest_type=0;
    }
    word |= ((unsigned short)inst->opcode << 8);
    word |= ((unsigned short)inst->funct << 4);
    word |= ((unsigned short)src_type << 2);
    word |= (unsigned short)dest_type;
    return word;
}
unsigned short encode_immediate(char *op) {
    int num;
    if (!is_valid_number(op + 1, &num)) {
        return 0;
    }
    return (unsigned short)(num & 0x0FFF);
}


unsigned short encode_register(char *op) {
    int reg_num;
    reg_num=findReg(op);
    if (reg_num<0){
        return 0;
    }
    return (unsigned short)(1<<reg_num);
}
void trim_spaces(char *str) {
    char *start = str;
    char *end;

    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    if (*str=='\0') {
        return;
    }
    end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}
int handle_one_operand(char *op,
                       int line_num,
                       LabelTable *labels,
                       CodeImage *code_img,
                       int *IC) {
    int operand_address;
    int type;
    int idx;
    unsigned short value;
    operand_address= *IC;
    type = get_addressing_type(op);
    if (type == ADDR_IMMEDIATE) {
        value=encode_immediate(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            return 0;
        }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_REGISTER) {
        value=encode_register(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            return 0;
        }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_DIRECT) {
        idx = findLabel(labels, op);
        if (idx != -1 &&
            labels->arr[idx].is_data == 0 &&
            labels->arr[idx].is_extern == 0) {
            if (!add_code_word(code_img,
                               (unsigned short)labels->arr[idx].address,
                               op,
                               line_num)) {
                return 0;
                               }
            } else {
                if (!add_code_word(code_img, 0, op, line_num)) {
                    return 0;
                }
            }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_RELATIVE) {
        char rel_label[31];
        strcpy(rel_label, op + 1);
        idx = findLabel(labels, rel_label);
        if (idx != -1 &&
            labels->arr[idx].is_data == 0 &&
            labels->arr[idx].is_extern == 0) {
            if (!add_code_word(code_img,
                               (unsigned short)(labels->arr[idx].address - operand_address),
                               NULL,
                               line_num)) {
                return 0;}
            } else {
                if (!add_code_word(code_img, 0, op, line_num)) {
                    return 0;
                }
            }
        (*IC)++;
        return 1;
    }
    return 0;
}
int is_one_of(int type, int a, int b, int c, int d) {
    return (type == a || type == b || type == c || type == d);
}
int is_legal_addressing(Instruction *inst, int src_type, int dest_type, int op_count) {
    if (inst == NULL) {
        return 0;
    }
    switch (inst->opcode) {
        case 0: /* mov */
            return op_count == 2 &&
                   is_one_of(src_type, 0, 1, 3, -1) &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 1: /* cmp */
            return op_count == 2 &&
                   is_one_of(src_type, 0, 1, 3, -1) &&
                   is_one_of(dest_type, 0, 1, 3, -1);

        case 2: /* add/sub */
            if (inst->funct == 10 || inst->funct == 11) {
                return op_count == 2 &&
                       is_one_of(src_type, 0, 1, 3, -1) &&
                       is_one_of(dest_type, 1, 3, -1, -1);
            }
            return 0;

        case 4: /* lea */
            return op_count == 2 &&
                   src_type == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 5: /* clr/not/inc/dec */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 9: /* jmp/bne/jsr */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 2, -1, -1);

        case 12: /* red */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 13: /* prn */
            return op_count == 1 &&
                   is_one_of(dest_type, 0, 1, 3, -1);

        case 14: /* rts */
        case 15: /* stop */
            return op_count == 0;

        default:
            return 0;
    }
}
int handle_instruction_line(char *line,int line_num,LabelTable *labels,CodeImage *code_img,int *IC,macro_node *macro_table,int macro_count){
    char temp[MAX_LINE_LENGTH];
    char *token;
    char label_name[31];
    char *operands_line;
    char op1[MAX_LINE_LENGTH];
    char op2[MAX_LINE_LENGTH];
    int has_label;
    int op_count;
    int src_type;
    int dest_type;
    Instruction *inst;
    unsigned short first_word;
    has_label =0;
    op_count =0;
    src_type=0;
    dest_type=0;
    strcpy(temp,line);
    token=strtok(temp," \t\n");
    if (token == NULL) {
        return 0;
    }
    /* אם יש label בתחילת שורה */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,labels,macro_table,macro_count)) {
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            return 0;
        }
    }
    inst=findInstruction(token);
    if (inst==NULL) {
        return 0;
    }
    if (has_label){
        if (!addLabel(labels,label_name,*IC,0,line_num)) {
            return 0;
        }
    }
    operands_line = strtok(NULL, "\n");

    op1[0] = '\0';
    op2[0] = '\0';

    if (operands_line != NULL) {
        if (strstr(operands_line, ",,") != NULL) {
            return 0;
        }
        if (operands_line[0] == ',') {
            return 0;
        }
        if (operands_line[strlen(operands_line) - 1] == ',') {
            return 0;
        }
    }
    parse_operands(operands_line,op1,op2,&op_count);

    if (op_count==-1) {
        return 0;
    }
    if (op_count!=inst->operand_count) {
        return 0;
    }

    if (op_count ==2) {
        src_type = get_addressing_type(op1);
        dest_type = get_addressing_type(op2);
        if (src_type == ADDR_INVALID || dest_type ==ADDR_INVALID) {
            return 0;
        }
        if (!is_legal_addressing(inst, src_type, dest_type, op_count)) {
            return 0;
        }
    }
    else if (op_count ==1) {
        dest_type = get_addressing_type(op1);
        if (dest_type == ADDR_INVALID) {
            return 0;
        }
        if (!is_legal_addressing(inst, 0, dest_type, op_count)) {
            return 0;
        }
    }
    else{
        if (!is_legal_addressing(inst,0,0,op_count)) {
            return 0;
        }
    }
    first_word=build_first_word(inst,op1,op2,op_count);
    if (!add_code_word(code_img,first_word,NULL,line_num)){
        return 0;
    }
    (*IC)++;

    if (op_count==0) {
        return 1;
    }
    if (op_count==1) {
        return handle_one_operand(op1,line_num,labels,code_img,IC);
    }

    if(op_count==2) {
        if (!handle_one_operand(op1,line_num,labels,code_img,IC)) {
            return 0;
        }
        if (!handle_one_operand(op2,line_num,labels,code_img,IC)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

void update_data_labels(LabelTable *labels, int IC) {
    int i;
    for (i=0; i<labels->count;i++) {
        if (labels->arr[i].is_data) {
            labels->arr[i].address+=IC;
        }
    }
}

int detect_line_type(char *line) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp,line);
    token = strtok(temp, " \t\n");
    if (token ==NULL) {
        return LINE_EMPTY; }
    if(strlen(token)> 0 && token[strlen(token)-1] == ':') {
        token = strtok(NULL," \t\n");
        if (token ==NULL) {
            return LINE_ERROR;
        }
    }
    if (strcmp(token,".entry") == 0) {
        return LINE_ENTRY;
    }
    if (strcmp(token,".extern") == 0) {
        return LINE_EXTERN;
    }
    if (strcmp(token,".data") == 0) {
        return LINE_DATA;
    }
    if (strcmp(token,".string") == 0) {
        return LINE_STRING;
    }
    if (findInstruction(token) != NULL) {
        return LINE_INSTRUCTION;
    }
    return LINE_ERROR;
}

int handle_extern_line(char *line, int line_num, LabelTable *labels, NameRefTable *externs, char *label_name,macro_node *macro_table,int macro_count) {
    char temp[MAX_LINE_LENGTH];
    char *token;
    strcpy(temp,line);
    token = strtok(temp," \t\n");
    if (token == NULL) {
        return 0; }
    if (token[strlen(token) -1] == ':') {
        return 0; }
    if (strcmp(token, ".extern") !=0) {
        return 0; }
    token = strtok(NULL, " \t\n");
    if (token ==NULL || !is_valid_label(token,labels,macro_table,macro_count)) {
        return 0; }
    strcpy(label_name, token);
    token = strtok(NULL, " \t\n");
    if (token!=NULL) {
        return 0; }
    if (!addLabel(labels, label_name, 0,0,line_num)) {
        return 0; }
    labels->arr[labels->count - 1].is_extern = 1;
    if (!add_name_ref(externs, label_name, line_num)) {
        return 0;
    }
    return 1;
}

int handle_first_pass_line(char *line,
                           int line_num,
                           LabelTable *labels,
                           CodeImage *code_img,
                           CodeImage *data_img,
                           NameRefTable *externs,
                           NameRefTable *entries,
                           int *IC,
                           int *DC,macro_node *macro_table,int macro_count) {
    int line_type;
    char extern_label[31];
    line_type=detect_line_type(line);
    switch(line_type) {
        case LINE_EMPTY:
            return 1;
        case LINE_ENTRY:
            return handle_entry_line(line,line_num,entries);
        case LINE_EXTERN:
            return handle_extern_line(line,line_num,labels,externs,extern_label,macro_table,macro_count);
        case LINE_DATA:
            return handle_data_line(line,line_num,labels,data_img,DC,labels,macro_table,macro_count);
        case LINE_STRING:
            return handle_string_line(line,line_num,labels,data_img,DC,labels,macro_table,macro_count);
        case LINE_INSTRUCTION:
            return handle_instruction_line(line,line_num,labels,code_img,IC,macro_table,macro_count);
        case LINE_ERROR:
        default:
            return 0;
    }
}
//int is_blank_or_commet(char *line);
//int is_valid_label(char *line);
int exe_first_pass(char *file_name,macro_node *macro_table,int macro_count){
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
    IC = 100;
    DC = 0;
    error_found = 0;

    if (!init_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries)) {
        printf("FAILED: init_first_pass_memory\n");
        return 0;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("FAILED: cannot open AM file: %s\n", file_name);
        free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
        return 0;
    }
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line_num++;
        if (is_blank_or_comment(line)) {
            continue;
        }
        if (!handle_first_pass_line(line, line_num, &labels, &code_img, &data_img,
                                    &externs, &entries, &IC, &DC,macro_table,macro_count)) {
            printf("First pass error in line %d: %s", line_num, line);
            error_found = 1;
                                    }
    }
    fclose(fp);
    update_data_labels(&labels,IC);
    if (!error_found) {
        printf("FIRST PASS REALLY PASSED. IC=%d DC=%d\n", IC, DC);
        if (!exe_sec_pass(file_name,&labels,&code_img,&data_img,&externs,&entries,IC,DC)) {
            printf("Second pass failed\n");
            error_found = 1;
        }
    }
    free_first_pass_memory(&labels, &code_img, &data_img, &externs, &entries);
    return !error_found;
}
