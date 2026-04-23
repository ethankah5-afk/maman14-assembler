//
// Created by ethan on 23/04/2026.
//

#ifndef ASSEMBLERPROJECT_SEC_PASS_H
#define ASSEMBLERPROJECT_SEC_PASS_H
#include "main_struct.h"

int exe_sec_pass(char *file_name,LabelTable *labels,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,NameRefTable *entries,int IC,int DC);

int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,int IC,int DC);

#endif //ASSEMBLERPROJECT_SEC_PASS_H
