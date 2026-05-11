/*
* Created by Ethan and Yakir
*/ 
#include <string.h>
#include <stdlib.h>
#include "main_struct.h"
#include "sec_pass.h"
#include "errors.h"
#include "constants.h"
#include "output.h"

/* 
* Mark entry labels in labels table
* labels - labels table
* entries - entries table
* file_name - source file name
* return - 1 if success otherwise 0 
*/ 

int mark_entry_labels(LabelTable *labels, NameRefTable *entries, char *file_name) {
    int i;
    int error_found;
    Label *lbl;
    location loc;

    loc.file_name = file_name;
    error_found=0;

    /* Go over all entry labels   */ 
    for(i=0; i<entries->count;i++) {
        lbl = find_label_by_name(labels, entries->arr[i].name);
        /* Entry label has no definition */ 
        if(lbl ==NULL) {
            print_external_error(ERROR_22,loc);
            error_found=1;
            continue;
        }
        
        /* Label cannot be both entry and extern  */ 
        if(lbl->is_extern) {
         print_external_error(ERROR_36,loc);
            error_found = 1;
            continue;
        }
        /* Mark the label as a entry */ 
        lbl->is_entry = 1;
    }
    return !error_found;
}

/* 
* Find label by name
* labels - labels table
* name - label name 
* return - pointer to label if found otherwise NULL 
*/ 
Label *find_label_by_name(LabelTable *labels, const char *name) {
    int i;

    /* Search labels table */ 
    for (i=0; i< labels->count; i++) {
        if(strcmp(labels->arr[i].symbol_name, name) ==0) {
            return &labels->arr[i];
        }
    }
    return NULL;
}

/* 
* check if name belongs to extern label
* externs - extern labels table
* name - label name
* return - 1 if extern otherwise 0 
*/ 
int is_extern_name(NameRefTable *externs, const char *name) {
    int i;

    /* Search extern labels table  */ 
    for ( i=0; i<externs->count; i++) {
        if(strcmp(externs->arr[i].name, name) == 0){
            return 1;
        }
    }
    return 0;
}

/* 
* Resolve one code word 
* word - code word 
* labels - labels table 
* externs - extern labels table
* file_name - source file name
* return - 1 if success otherwise 0 
*/ 
int resolve_one_code_word(CodeWord *word,int index, LabelTable *labels, NameRefTable *externs, char *file_name) {
    Label *lbl;
    int current_address;
    location loc;
    loc.file_name = file_name;
    loc.line_num = word->assembly_line;
    if(word->label == NULL) {
        return 1;
    }
    current_address = 100+index;

    /*  Relative label addressing */ 
    if (word->label[0]=='%') {
        lbl = find_label_by_name(labels, word->label+1);
        if (lbl ==NULL) {
             print_external_error(ERROR_22, loc);
        return 0;
        }
    /* Relative extern labels are external  */ 
    if (lbl->is_extern) {
            word->value = ARE_EXTERNAL;
            return 1;
        }
        /* store relative address */ 
        word->value=(unsigned short)(((lbl->address-current_address)<<2)|ARE_RELOCATABLE);
        return 1;
    }
    lbl = find_label_by_name(labels, word->label);

    /* not defined label  */ 
    if (lbl==NULL) {
        print_external_error(ERROR_22,loc);
        return 0;
    }
    /* external label  */ 
    if (is_extern_name(externs, word->label)) {
        word->value=ARE_EXTERNAL;
    }

        /*  Absolute label address */ 
    else {
    
        word->value = (lbl->address << 2) |ARE_RELOCATABLE;
    }
    return 1;
}

/* 
* Resolve all labels in code image 
* code_img - code image
* labels - labels table
* externs - extern labels table
* file_name - source file name 
* return - 1 if success otherwise 0 
*/ 
int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs, char *file_name) {
    int i;
    int error_found =0;
    
    /* resolve every code word  */ 
    for(i=0; i<code_img->count; i++) {
        if (!resolve_one_code_word(&code_img->arr[i],i,labels, externs, file_name)) {
               error_found = 1;
        }
    }
    return !error_found;
}

/* 
* Execute second assembler pass
* file_name - source file name
* labels - labels table
* code_img - code image
* data_img - data image
* externs - extern labels table 
* entries - entries table 
* IC - final instruction counter 
* DC - final data counter 
* return - 1 if success otherwise 0 
*/ 
int exe_sec_pass(char *file_name, LabelTable *labels, CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC) {
    int error_found=0;

    /* Resolve all label addresses  */ 
    if (!resolve_code_labels(code_img,labels,externs,file_name)) {
        error_found=1;
    }
    /* Mark entry labels  */  
    if (!mark_entry_labels(labels,entries, file_name)) {
        error_found=1;
    } 
    /* Create output files only if there are no errors  */ 
    if (!error_found) {
        if (!write_ob_file(file_name,code_img,data_img,externs,IC,DC)) {
            error_found=1;
        }
        if (!write_ext_file(file_name,code_img,externs)){
            error_found=1;
        }
        if (!write_ent_file(file_name,labels)){
            error_found=1;
        }
    }
    return !error_found;
}
