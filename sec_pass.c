//
// Created by ethan on 23/04/2026.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "first_pass.h"
#include "main_struct.h"
#include "sec_pass.h"

#define ARE_EXTERNAL 1
#define ARE_RELOCATABLE 2

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
int resolve_one_code_word(CodeWord *word, LabelTable *labels, NameRefTable *externs) {
    Label *lbl;
    if(word->label == NULL) {
        return 1;
    }
    lbl = find_label_by_name(labels, word->label);
    if (lbl ==NULL) {
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

int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs) {
    int i;
    for(i=0; i<code_img->count; i++) {
        if (!resolve_one_code_word(&code_img->arr[i],labels, externs)) {
            return 0;
        }
    }
    return 1;
}
int mark_entry_labels(LabelTable *labels, NameRefTable *entries) {
    int i;
    Label *lbl;
    for(i=0; i<entries->count;i++) {
        lbl = find_label_by_name(labels, entries->arr[i].name);
        if(lbl ==NULL)
            return 0;
        if(lbl->is_extern)
            return 0;
        lbl->is_entry = 1;
    }
    return 1;
}
int write_ent_file(char *file_name,LabelTable *labels) {
    int i;
    int found=0;
    FILE *fp;
    char ent_name[256];
    sprintf(ent_name,"%s.ent",file_name);
    fp=fopen(ent_name,"w");
    if (fp==NULL) {
        return 0;
    }
    for (i=0; i<labels->count; i++) {
        if (labels->arr[i].is_entry) {
            fprintf(fp,"%s %d\n",labels->arr[i].symbol_name,labels->arr[i].address);
            found=1;
        }
    }
    fclose(fp);
    return 1;
}
int write_ext_file(char *file_name, CodeImage *code_img, LabelTable *labels) { 
    int i; 
    FILE *fp; 
    char ext_name[256]; 
    sprintf(ext_name, "%s.ext", file_name); 
    fp = fopen(ext_name, "w"); 
    if (fp==NULL) { 
        return 0;
    } 
    for (i=0; i<code_img->count;i++) { 
        CodeWord *word = &code_img->arr[i];
        if(word->label !=NULL) { 
            Label *lbl = find_label_by_name(labels, word->label); 
            if (lbl!=NULL && lbl->is_extern) { 
                fprintf(fp,"%s %d\n", word->label, 100+i); 
            }
        }
    }
    fclose(fp); 
    return 1; 
}
int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,int IC, int DC) {
    FILE *fp;
    char ob_name[100];
    int i;
    make_output_name(file_name,".ob",ob_name);
    fp=fopen(ob_name,"w");
    if (fp==NULL) {
        return 0;
    }
    fprintf(fp," %d  %d\n",IC,DC);
    for (i=0;i<code_img->count;i++) {
        fprintf(fp,"%s\n",convert_word_to_output(code_img->arr[i].value));
    }
    for (i=0;i<data_img->count;i++) {
        fprintf(fp,"%s\n",convert_word_to_output(data_img->arr[i].value));
    }
    fclose(fp);
    return 1;
}
/*int collect_entries_output(LabelTable *labels, FILE *fp) {
    int i;
    for (i=0; i<labels->count; i++) {
        if (labels->arr[i].is_entry) {
            printf("ENTRY: %s %d\n", labels->arr[i].symbol_name, labels->arr[i].address);  }
    }
}*/
int exe_sec_pass(char *file_name, LabelTable *labels, CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC) {
    int error_found=0;
    if (!resolve_code_lables(code_img,labels,externs)) {
        error_found=1;
    }
    if (!mark_entry_labels(labels,entries)) {
        error_found=1;
    }
    if (!error_found) {
        if (!write_ob_file(file_name,code_img,data_img,IC,DC)) {
            error_found=1;
        }
        if (!write_ext_file(file_name,code_img,labels)){
            error_found=1;
        }
        if (!write_ent_file(file_name,labels)){
            error_found=1;
        }
    }
    return !error_found;
}
