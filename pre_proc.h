/*
* Created by Ethan and Yakir 
*/
#ifndef ASSEMBLERPROJECT_PREPROC_H
#define ASSEMBLERPROJECT_PREPROC_H
#include <stdio.h>
#include "errors.h"
#include "main_struct.h"

/* 
* Find macro by name
* table - macros table
* table_size - number of macros
* name - macro name 
* return - 1 if found otherwise -1
*/
int find_macro(macro_node *table, int table_size, char *name);

/* 
* Free macro table memory
* table - macros table 
* table_size - number of macros 
*/
void free_macro_table(macro_node *table, int table_size);

/* 
* Build file name with new extension
* file_name - source file name 
* ending - new extension
* return - new file name
*/
char *add_new_file(char *file_name,char *ending);

/* 
* check if line defines a macro
* line_num - source line
* mcro_name - returned macro name
* error_code - returned error code
* return - status code
*/

int is_macro(char *line_num,char **mcro_name,ERROR_CODES *error_code);

/*
* Save macro content from file
* fp - source file 
* pos - position in file
* line_count - current line counter
* error_code - returned error code
* return - macro content
*/
char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count,ERROR_CODES *error_code);

/*
* check if line is macro call 
* line - source line
* macro_name - macro name
* return - 1 if is macro call otherwise 0 
*/
int is_macro_call(char *line,char *macro_name);

/*
* Run preprocessor on assembly file
* file_name - source file name
* macro_table - returned macros table 
* macro_count - returned number of macros
* return - 1 if success if not 0
*/
int run_preproc(char *file_name,macro_node **macro_table,int *macro_count);

#endif  /* ASSEMBLERPROJECT_PREPROC_H */
