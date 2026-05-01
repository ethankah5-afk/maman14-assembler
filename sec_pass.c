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
#include "conversion.h"
#define ARE_EXTERNAL 1
#define ARE_RELOCATABLE 2
#define IC_INIT_VALUE 100
void make_output_name(char *file_name, char *ext, char *out) {
    char *dot;
    strcpy(out,file_name);
    dot = strrchr(out,'.');
    if (dot!= NULL)
        *dot = '\0';
    strcat(out,ext);
}
int mark_entry_labels(LabelTable *labels, NameRefTable *entries) {
    int i;
    int error_found=0;
    Label *lbl;
    for(i=0; i<entries->count;i++) {
        lbl = find_label_by_name(labels, entries->arr[i].name);
        if(lbl ==NULL) {
            printf("Error: entry label '%s' was not defined\n", entries->arr[i].name);
            error_found=1;
            continue;
        }
        if(lbl->is_extern) {
            printf("Error: label '%s' cannot be both entry and extern\n", entries->arr[i].name);
            error_found = 1;
            continue;
        }
        lbl->is_entry = 1;
    }
    return !error_found;
}
char *convert_word_to_hex(unsigned short word,char res[4]) {
 char hex_table[] = "0123456789ABCDEF";
    res[0] = hex_table[(word >> 8) & 0xF];
    res[1] = hex_table[(word >> 4) & 0xF];
    res[2] = hex_table[word & 0xF];
    res[3] = '\0';

        return res;
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
int resolve_one_code_word(CodeWord *word,int index, LabelTable *labels, NameRefTable *externs) {
    Label *lbl;
    int current_address;
    if(word->label == NULL) {
        return 1;
    }
    current_address=100+index;
    if (word->label[0]=='%') {
        lbl = find_label_by_name(labels, word->label+1);
        if (lbl ==NULL||lbl->is_extern) {
            return 0;
        }
        word->value=(unsigned short)(((lbl->address-current_address)<<2)|ARE_RELOCATABLE);
        return 1;
    }
    lbl = find_label_by_name(labels, word->label);
    if (lbl==NULL) {
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
        if (!resolve_one_code_word(&code_img->arr[i],i,labels, externs)) {
            return 0;
        }
    }
    return 1;
}
int write_ent_file(char *file_name,LabelTable *labels) {
    int i;
    FILE *fp;
    char ent_name[256];
    for(i=0; i<(labels->count);i++) {
        if(labels->arr[i].is_entry) {
            break;
        }
    }
    if (i==(labels->count)) {
        return 1;
    }
    make_output_name(file_name,".ent", ent_name);
    fp = fopen(ent_name, "w");
    if (fp==NULL) {
        return 0;
    }
    for (i=0; i<labels->count; i++) {
        if (labels->arr[i].is_entry) {
            fprintf(fp,"%s %d\n",labels->arr[i].symbol_name,labels->arr[i].address);
        }
    }
    fclose(fp);
    return 1;
}
int write_ext_file(char *file_name, CodeImage *code_img,NameRefTable *externs) {
    int i;
    int found=0;
    FILE *fp;
    char ext_name[256];
    make_output_name(file_name,".ext",ext_name);
    fp = fopen(ext_name, "w");
    if (fp==NULL) {
        return 0;
    }
    found=0;
    for (i=0; i<code_img->count;i++) {
        if (code_img->arr[i].label!=NULL&&is_extern_name(externs,code_img->arr[i].label)) {
            fprintf(fp, "%s %d\n",code_img->arr[i].label,100+i);
            found=1;
        }
    }
    fclose(fp);
    if (!found) {
        remove(ext_name);
    }
    return 1;
}
int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,int IC, int DC) {
    FILE *fp;
    char ob_name[100];
    int i;
    char out_word[4];
    make_output_name(file_name,".ob",ob_name);
    fp=fopen(ob_name,"w");
    if (fp==NULL) {
        return 0;
    }
    fprintf(fp,"%d %d \n", IC -IC_INIT_VALUE, DC);
    for(i=0; i< code_img->count; i++) {
        convert_word_to_hex(code_img->arr[i].value,out_word);
        fprintf(fp, "%04d %s\n",IC_INIT_VALUE+i, out_word);
    }
    for(i=0; i<data_img->count; i++) {
        convert_word_to_hex(data_img->arr[i].value,out_word);
        fprintf(fp, "%04d %s\n",IC_INIT_VALUE+code_img->count+i, out_word);
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
    if (!resolve_code_labels(code_img,labels,externs)) {
        error_found=1;
    }
    if (!mark_entry_labels(labels,entries)) {
        error_found=1;
    }
    if (!error_found) {
        if (!write_ob_file(file_name,code_img,data_img,IC,DC)) {
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
