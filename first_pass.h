//
// Created by ethan on 15/03/2026.
//

#ifndef ASSEMBLERPROJECT_FIRST_PASS_H
#define ASSEMBLERPROJECT_FIRST_PASS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "main_struct.h"
#include "errors.h"

#define MAX_LINE_LENGTH 81
int parse_data_values(char *args, CodeImage *data_img, int line_num, int *DC, ERROR_CODES *error_code);
int is_blank_or_comment(const char *line);
int is_valid_number(const char *token, int *value);
int is_valid_label(char *label_name, LabelTable *table,macro_node *macro_table,int macro_size);
int is_label_operands(char *label_name);
int initLabelTable(LabelTable *table);
int findLabel(LabelTable *table, char *name);
int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined);
int add_code_word(CodeImage *img, unsigned short value, const char *label, int line);
void free_code_image(CodeImage *img);
int init_code_image(CodeImage *img);
int init_name_ref_table(NameRefTable *table);
void free_name_ref_table(NameRefTable *table);
int add_name_ref(NameRefTable *table, const char *name, int line);
void free_label_table(LabelTable *table);
int handle_entry_line(char *line, int line_num, char*file_name,  NameRefTable *entries);
int handle_data_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count);
int handle_string_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count);
unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count);
unsigned short encode_immediate(char *op);
int init_first_pass_memory(LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries);
void free_first_pass_memory(LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries);
void trim_spaces(char *str);
void parse_operands(char *operands_line, char *op1, char *op2, int *count);
int handle_instruction_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC,macro_node *macro_table,int macro_count);
int handle_one_operand(char *op,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC);
void update_data_labels(LabelTable *labels, int IC);
int handle_first_pass_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int *IC,int *DC,macro_node *macro_table,int macro_count);
int exe_passes(char *file_name,macro_node *macro_table,int macro_size);
int is_one_of(int type, int a, int b, int c, int d);
int is_legal_addressing(Instruction *inst, int src_type, int dest_type, int op_count);
int detect_line_type(char *line);
int handle_extern_line(char *line, int line_num,char *file_name, LabelTable *labels, NameRefTable *externs, char *label_name,macro_node *macro_table,int macro_count);
#endif //ASSEMBLERPROJECT_FIRST_PASS_H
