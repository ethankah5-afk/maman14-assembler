/*
* Created by Ethan and Yakir
*/ 

#ifndef ASSEMBLERPROJECT_SEC_PASS_H
#define ASSEMBLERPROJECT_SEC_PASS_H
#include "main_struct.h"

/* 
* Find label by name
* labels - labels table
* name - label name 
* return - pointer to label if found otherwise NULL 
*/
Label *find_label_by_name(LabelTable *labels, const char *name);

/* 
* check if name belongs to extern label
* externs - extern labels table
* name - label name
* return - 1 if extern otherwise 0 
*/ 
int is_extern_name(NameRefTable *externs, const char *name);


/* 
* Resolve one code word 
* word - code word 
* labels - labels table 
* externs - extern labels table
* file_name - source file name
* return - 1 if success otherwise 0 
*/ 
int resolve_one_code_word(CodeWord *word,int index, LabelTable *labels, NameRefTable *externs,char *file_name);


/* 
* Resolve all labels in code image 
* code_img - code image
* labels - labels table
* externs - extern labels table
* file_name - source file name 
* return - 1 if success otherwise 0 
*/ 
int resolve_code_labels(CodeImage *code_img, LabelTable *labels, NameRefTable *externs,char *file_name);

/* 
* Mark entry labels in labels table
* labels - labels table
* entries - entries table
* file_name - source file name
* return - 1 if success otherwise 0 
*/ 
int mark_entry_labels(LabelTable *labels, NameRefTable *entries,char *file_name);

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
int exe_sec_pass(char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC);

#endif  /* ASSEMBLERPROJECT_SEC_PASS_H */  
