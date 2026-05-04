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
    macro_node *macro_table=NULL;
    int macro_count=0;
    int i;

    if (argc < 2) { 
        print_internal_error(ERROR_2);
        return 0; 
    }
    
    for (i = 1; i < argc; i++) {
        
        char *as_file;
        char *am_file;
        
        as_file = add_new_file(argv[i], ".as");

        if(as_file ==NULL) {   
        print_internal_error(ERROR_1);
        return 0;    
        }

        if (!run_preproc(as_file,&macro_table,&macro_count)) {
            print_internal_error(ERROR_15);
            
        if (macro_table != NULL) {
            
            free_macro_table(macro_table, macro_count);
            macro_table = NULL;
            macro_count = 0;}
    
            free(as_file);
            continue;
        }
        /* שלב 2: first pass */
        am_file = add_new_file(argv[i], ".am");

    if(am_file == NULL) {
        
        print_internal_error(ERROR_1);
         
        free_macro_table(macro_table, macro_count);
        macro_table = NULL;
        macro_count = 0;

        free(as_file);
        return 0;

    }

 
        exe_first_pass_and_second_pass(am_file,macro_table,macro_count);
        
        free_macro_table(macro_table,macro_count);
        macro_table=NULL;
        macro_count=0;
        free(as_file);
        free(am_file);
    }
    return 0;
}
