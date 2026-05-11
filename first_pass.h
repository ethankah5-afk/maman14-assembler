/* 
* Created by Ethan and Yakir 
*/

#ifndef ASSEMBLERPROJECT_FIRST_PASS_H
#define ASSEMBLERPROJECT_FIRST_PASS_H
#include "main_struct.h"

/*
* Handle .entry line 
* line - source line 
* line_num - current line number 
* file_name - source file name 
* entries - entries table 
* return - status of the code 
*/
int handle_entry_line(char *line, int line_num, char*file_name,  NameRefTable *entries);

/*
* Handle .data line 
* line - source line 
* line_num - current line number
* file_name - source file name 
* labels - labels table 
* data_img - data image 
* DC - data counter 
* table - labels table 
* macro_table - macros table
* macro_count - number of macros
* return - status of the code 
*/
int handle_data_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count);

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
* macro_count - number of macros 
* return - status code 
*/
int handle_string_line(char *line, int line_num,char *file_name, LabelTable *labels, CodeImage *data_img, int *DC,LabelTable *table,macro_node *macro_table,int macro_count);

/* 
* Initialize first pass memory structures
* labels - labels table 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* entries - entries table 
* return - 1 if success if not 0 
*/
int init_first_pass_memory(LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries);

/* 
* Free first pass memory structures
* labels - labels table 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* entries - entries table 
*/
void free_first_pass_memory(LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries);

/* 
* Update data labels address after first pass
* labels - labels table 
* IC - final instruction counter
*/
void update_data_labels(LabelTable *labels, int IC);

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

int handle_first_pass_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int *IC,int *DC,macro_node *macro_table,int macro_count);

/* 
* Execute first and second assembler passes
* file_name - source file name 
* macro_table - macros table 
* macro_count - number of macros 
* return - 1 if assembler passes succeeded otherwise 0 
*/
int exe_passes(char *file_name,macro_node *macro_table,int macro_size);

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
int handle_extern_line(char *line, int line_num,char *file_name, LabelTable *labels, NameRefTable *externs, char *label_name,macro_node *macro_table,int macro_count);

#endif /* ASSEMBLERPROJECT_FIRST_PASS_H  */
