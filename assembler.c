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
    int i;
    for (i = 1; i < argc; i++) {
        char *as_file;
        char *am_file;
        /* שלב 1: preproc */
        as_file = add_new_file(argv[i], ".as");
        if (!run_preproc(as_file)) {
            printf("Preproc failed\n");
            free(as_file);
            continue;
        }
        /* שב 2: first pass */
        am_file = add_new_file(argv[i], ".am");
        if (!exe_first_pass(am_file)) {
            printf("First pass failed\n");
        }
        free(as_file);
        free(am_file);
    }
    return 0;
}
// #include <stdio.h>
#include <stdlib.h>
#include "pre_proc.h"
#include "first_pass.h"
#include "main_struct.h"

int main(int argc, char *argv[])
{
    char *as_file;
    char *am_file;
    node *macro_table;

    while (--argc > 0) {
        macro_table = NULL;

        printf("Start pre-proc\n");
        as_file = add_new_file(argv[argc], ".as");

        if (!run_preproc(as_file, &macro_table)) {
            printf("Preproc failed\n");
            free_macro_list(macro_table);
            free(as_file);
            continue;
        }

        printf("Start first pass\n");
        am_file = add_new_file(argv[argc], ".am");

        if (!exe_first_pass(am_file, macro_table)) {
            printf("First pass failed\n");
            free_macro_list(macro_table);
            free(as_file);
            free(am_file);
            continue;
        }

        free_macro_list(macro_table);
        free(as_file);
        free(am_file);
    }

    printf("end\n");
    return 0;
}
