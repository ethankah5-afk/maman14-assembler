//
// Created by ethan on 08/05/2026.
//
#include <stdio.h>
#include <string.h>
#include "main_struct.h"
#include "errors.h"
#include "sec_pass.h"
#include "output.h"
#include "constants.h"

void make_output_name(char *file_name, char *ext, char *out) {
    char *dot;
    strcpy(out,file_name);
    dot = strrchr(out,'.');
    if (dot!= NULL)
        *dot = '\0';
    strcat(out,ext);
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
        print_internal_error(ERROR_7);
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
        print_internal_error(ERROR_7);
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
        print_internal_error(ERROR_7);
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