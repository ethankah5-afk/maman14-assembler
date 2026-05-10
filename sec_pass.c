//
// Created by ethan on 23/04/2026.
//
#include <string.h>
#include <stdlib.h>
#include "main_struct.h"
#include "sec_pass.h"
#include "errors.h"
#include "constants.h"
#include "output.h"

int mark_entry_labels(LabelTable *labels, NameRefTable *entries, char *file_name) {
    int i;
    int error_found;
    Label *lbl;
    location loc;

    loc.file_name = file_name;
    error_found=0;

    for(i=0; i<entries->count;i++) {
        lbl = find_label_by_name(labels, entries->arr[i].name);
        if(lbl ==NULL) {
            print_external_error(ERROR_22,loc);
            error_found=1;
            continue;
        }
        if(lbl->is_extern) {
         print_external_error(ERROR_36,loc);
            error_found = 1;
            continue;
        }
        lbl->is_entry = 1;
    }
    return !error_found;
}

Label *find_label_by_name(LabelTable *labels, const char *name) {
    int i;
    for (i=0; i< labels->count; i++) {
        if(strcmp(labels->arr[i].symbol_name, name) ==0) {
            return &labels->arr[i];
        }
    }
    return NULL;
}

int is_extern_name(NameRefTable *externs, const char *name) {
    int i;
    for ( i=0; i<externs->count; i++) {
        if(strcmp(externs->arr[i].name, name) == 0){
            return 1;
        }
    }
    return 0;
}
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
    if (word->label[0]=='%') {
        lbl = find_label_by_name(labels, word->label+1);
        if (lbl ==NULL) {
             print_external_error(ERROR_22, loc);
        return 0;
        }

    if (lbl->is_extern) {
            word->value = ARE_EXTERNAL;
            return 1;
        }
        word->value=(unsigned short)(((lbl->address-current_address)<<2)|ARE_RELOCATABLE);
        return 1;
    }
    lbl = find_label_by_name(labels, word->label);
    if (lbl==NULL) {
        print_external_error(ERROR_22,loc);
        return 0;
    }
    if (is_extern_name(externs, word->label)) {
        word->value=ARE_EXTERNAL;
    }
    else {
        word->value = (lbl->address << 2) |ARE_RELOCATABLE;
    }
    return 1;
}

int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs, char *file_name) {
    int i;
    int error_found =0;
    for(i=0; i<code_img->count; i++) {
        if (!resolve_one_code_word(&code_img->arr[i],i,labels, externs, file_name)) {
               error_found = 1;
        }
    }
    return !error_found;
}

int exe_sec_pass(char *file_name, LabelTable *labels, CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC) {
    int error_found=0;
    if (!resolve_code_labels(code_img,labels,externs,file_name)) {
        error_found=1;
    }
    if (!mark_entry_labels(labels,entries, file_name)) {
        error_found=1;
    }
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
