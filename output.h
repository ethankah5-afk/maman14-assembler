//
// Created by ethan on 08/05/2026.
//

#ifndef ASSEMBLERPROJECT_OUTPUT_H
#define ASSEMBLERPROJECT_OUTPUT_H
#include "main_struct.h"

void make_output_name(char *file_name, char *ext, char *out);
int write_ent_file(char *file_name,LabelTable *labels);
int write_ext_file(char *file_name, CodeImage *code_img,NameRefTable *externs);
int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,int IC, int DC);

#endif //ASSEMBLERPROJECT_OUTPUT_H