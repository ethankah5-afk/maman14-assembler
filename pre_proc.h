//
// Created by ethan on 09/03/2026.
//
#ifndef ASSEMBLERPROJECT_PREPROC_H
#define ASSEMBLERPROJECT_PREPROC_H
#include <stdio.h>

#include "errors.h"
#include "main_struct.h"
int find_macro(macro_node *table, int table_size, char *name);
void free_macro_table(macro_node *table, int table_size);
char *add_new_file(char *file_name,char *ending);
int is_macro(char *line_num,char **mcro_name,ERROR_CODES *error_code);
char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count,ERROR_CODES *error_code);
int is_macro_call(char *line,char *macro_name);
int run_preproc(char *file_name,macro_node **macro_table,int *macro_count);
#endif //ASSEMBLERPROJECT_PREPROC_H
