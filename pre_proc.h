//
// Created by ethan on 09/03/2026.
//
#ifndef ASSEMBLERPROJECT_PREPROC_H
#define ASSEMBLERPROJECT_PREPROC_H
#include <stdio.h>

int is_macro(char *line_num,char **mcro_name);
char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count);
int is_macro_call(char *line,char *macro_name);
int run_preproc(char *file_name);

#endif //ASSEMBLERPROJECT_PREPROC_H
