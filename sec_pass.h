//
// Created by ethan on 23/04/2026.
//

#ifndef ASSEMBLERPROJECT_SEC_PASS_H
#define ASSEMBLERPROJECT_SEC_PASS_H
#include "main_struct.h"
#include <stdio.h>

Label *find_label_by_name(LabelTable *labels, const char *name);
int is_extern_name(NameRefTable *externs, const char *name);
int resolve_one_code_word(CodeWord *word, LabelTable *labels, NameRefTable *externs);
int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs);
int mark_entry_labels(LabelTable *labels, NameRefTable *entries);
int exe_sec_pass(char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC);
int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,int IC,int DC);
int write_ext_file(char *file_name, CodeImage *code_img, LabelTable *labels);
int write_ent_file(char *file_name, LabelTable *labels);


#endif //ASSEMBLERPROJECT_SEC_PASS_H
