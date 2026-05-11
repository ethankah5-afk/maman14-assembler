/*
* Created by Ethan and Yakir
*/
#include <stdio.h>
#include <ctype.h>
#include "first_pass.h"
#include <stdlib.h>
#include "main_struct.h"
#include "pre_proc.h"

/* 
* Main function of the assembler project 
* argc - number of command line arguments
* argv - command line arguments array 
*/

int main(int argc, char *argv[]) {
    macro_node *macro_table=NULL;
    int macro_count=0;
    int i;
/* check that at least one file was received */
    if (argc < 2) {
        print_internal_error(ERROR_2);
        return 0;
    }
/* Go over all the files received from the command line */
    for (i = 1; i < argc; i++) {

        char *as_file;
        char *am_file;
        
    /* Create as. file name */
        as_file = add_new_file(argv[i], ".as");

        if(as_file ==NULL) {
            print_internal_error(ERROR_1);
            return 0;
        }
        
/* Run preprocessor stage: expands macros and creates .am file  */
        if (!run_preproc(as_file,&macro_table,&macro_count)) {
            
            /* Free macro table if preprocessing failed */
            if (macro_table != NULL) {
                free_macro_table(macro_table, macro_count);
                macro_table = NULL;
                macro_count = 0;}
            free(as_file);
            continue;
        }
        /* Create .am file name */
        am_file = add_new_file(argv[i], ".am");
        if(am_file == NULL) {
            print_internal_error(ERROR_1);
            free_macro_table(macro_table, macro_count);
            macro_table = NULL;
            macro_count = 0;
            free(as_file);
            return 0;
        }
        /* Run assembler passes on .am file */
        exe_passes(am_file,macro_table,macro_count);
        /* Free macro table after finishing current file */
        free_macro_table(macro_table,macro_count);
        macro_table=NULL;
        macro_count=0;
        free(as_file);
        free(am_file);
    }
    return 0;
}
