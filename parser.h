/* 
* Created by Ethan and Yakir 
*/

#ifndef ASSEMBLERPROJECT_PARSER_H
#define ASSEMBLERPROJECT_PARSER_H
#include "main_struct.h"
#include "pre_proc.h"

/*
* Remove spaces (Leading and Trailing) 
* str - string to trim 
*/
void trim_spaces(char *str);

/* 
* check if line is empty or comment
* line - source line
* return - 1 if is blank/comment otherwise 0 
*/
int is_blank_or_comment(const char *line);

/*
* Detect assembly line type 
* line - source line 
* return - detected line type
*/
int detect_line_type(char *line);

/*
* Parse instruction operands 
* operands_line - operands string
* op1 - first operand 
* op2 - second operand 
* count - number of operands found
*/
void parse_operands(char *operands_line, char *op1, char *op2, int *count);

/* 
* Parse .data values
* operands - data operands string 
* data_img - data image
* line_num - current line number 
* DC - data counter 
* error_code - returned error code 
* return -status of the code
*/
int parse_data_values(char *operands, CodeImage *data_img, int line_num, int *DC, ERROR_CODES *error_code);

/*
* check if the number is valid 
* token - number string
* value - number
* return - 1 if number is valid otherwise 0 
*/
int is_valid_number(const char *token, int *value);

/* 
* Validate label declaration 
* label_name - label name 
* table - labels table
* macro_table - macros table
* macro_count - number of macros 
* return - 1 if valid otherwise 0
*/
int is_valid_label(char *label_name, LabelTable *table,macro_node *macro_table,int macro_size);

/* 
* Validate label operand 
* label_name - label name 
* return - 1 if valid otherwise 0
*/
int is_label_operands(char *label_name);

#endif  /* ASSEMBLERPROJECT_PARSER_H */ 
