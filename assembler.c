//
// Created by ethan on 25/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include <stdlib.h>
#include "main_struct.h"
#include "pre_proc.h"
int main(int argc, char *argv[]) {
    
    node *macro_table; 
    macro_table = NULL; 
    
    int i;

    for (i = 1; i < argc; i++) {

        char *as_file;
        char *am_file;

        /* שלב 1: preproc */
        as_file = add_new_file(argv[i], ".as");

        if (!run_preproc(as_file,&macro_table)) {
            printf("Preproc failed\n");
            free(as_file);
            free_macro_list(macro_table);
            continue;
        }
        /* שלב 2: first pass */
        am_file = add_new_file(argv[i], ".am");

        if (!exe_first_pass(am_file),macro_table) {
        free(as_file);
        free(am_file);
        free_macro_list(macro_table);
        printf("First pass failed\n");
            continue;
        }

        free(as_file);
        free(am_file);
        free_macro_list(macro_table);
    }

    return 0;
}
