//
// Created by ethan on 08/05/2026.
//
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"
#include "constants.h"
#include "tables.h"


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
int parse_data_values(char *operands, CodeImage *data_img, int line_num, int *DC, ERROR_CODES *error_code){
    char *p;
    char *endptr;
    long num;
    p = operands;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0' || *p == '\n') {
        *error_code = ERROR_49; /* no .data arguments */
        return 0;
    }
    while (*p != '\0' && *p != '\n') {
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == ',') {
            *error_code = ERROR_39; /* too many commas / comma in wrong place */
            return 0;
        }
        num = strtol(p, &endptr, 10);
        if (p == endptr) {
            *error_code = ERROR_50; /* not a number */
            return 0;
        }
        if (num < -2048 || num > 2047) {
            *error_code = ERROR_57; /* out of range */
            return 0;
        }
        while (*endptr == ' ' || *endptr == '\t') {
            endptr++;
        }
        if (*endptr == '\0' || *endptr == '\n') {
            if (!add_code_word(data_img, (unsigned short)num, NULL, line_num)) {
                *error_code = ERROR_1;
                return -1;
            }
            (*DC)++;
            return 1;
        }
        if (*endptr != ',') {
            *error_code = ERROR_43;
            return 0;
        }
        endptr++;
        while (*endptr == ' ' || *endptr == '\t') {
            endptr++;
        }
        if (*endptr == '\0' || *endptr == '\n') {
            *error_code = ERROR_51;
            return 0;
        }
        if (!add_code_word(data_img, (unsigned short)num, NULL, line_num)) {
            *error_code = ERROR_1;
            return -1;
        }
        (*DC)++;
        p = endptr;
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