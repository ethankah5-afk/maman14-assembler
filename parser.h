//
// Created by ethan on 08/05/2026.
//

#ifndef ASSEMBLERPROJECT_PARSER_H
#define ASSEMBLERPROJECT_PARSER_H
#include "main_struct.h"
#include "pre_proc.h"
void trim_spaces(char *str);
int is_blank_or_comment(const char *line);
int detect_line_type(char *line);
void parse_operands(char *operands_line, char *op1, char *op2, int *count);
int parse_data_values(char *operands, CodeImage *data_img, int line_num, int *DC, ERROR_CODES *error_code);
int is_valid_number(const char *token, int *value);
int is_valid_label(char *label_name, LabelTable *table,macro_node *macro_table,int macro_size);
int is_label_operands(char *label_name);
#endif //ASSEMBLERPROJECT_PARSER_H