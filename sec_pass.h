//
// Created by ethan on 23/04/2026.
//

#ifndef ASSEMBLERPROJECT_SEC_PASS_H
#define ASSEMBLERPROJECT_SEC_PASS_H
#include "main_struct.h"
Label *find_label_by_name(LabelTable *labels, const char *name);
int is_extern_name(NameRefTable *externs, const char *name);
int resolve_one_code_word(CodeWord *word,int index, LabelTable *labels, NameRefTable *externs,char *file_name);
int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs,char *file_name);
int mark_entry_labels(LabelTable *labels, NameRefTable *entries,char *file_name);
int exe_sec_pass(char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC);

#endif //ASSEMBLERPROJECT_SEC_PASS_H
