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
