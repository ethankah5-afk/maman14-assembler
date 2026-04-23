//
// Created by ethan on 23/04/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include "first_pass.c"
#include <stdlib.h>
#include "main_struct.h"
#include "sec_pass.h"

int exe_sec_pass() {
    int i;
    if (error_found) {
        return 0;
    }
    if (IC>IC_MAX) {
        print_internal_error(ERROR_CODE_54);
        return 0;
    }
    for(){

    }
    return 1;
}







Label *find_label_by_name(LableTable *labels, const char *name) {  

int i; 
    
for (i=0; i< labels->size; i++) { 
    if(strcmp(labels->arr[i].symbol_name, name) ==0) {
        return &labels->arr[i]; 
    )
        
return NULL; 
}


int is_extern_name(NameRefTable *externs, const char *name) { 

    int i;

    for ( i=0; i<(externs->size); i++) { 
    if(strcmp(externs->arr[i].name, name) == 0) 
        return 1;
    }
return 0; 
}


























