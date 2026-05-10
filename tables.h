//
// Created by ethan on 08/05/2026.
//

#ifndef ASSEMBLERPROJECT_TABLES_H
#define ASSEMBLERPROJECT_TABLES_H
#include "main_struct.h"
int initLabelTable(LabelTable *table);
int findLabel(LabelTable *table, char *name);
int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined);
int add_code_word(CodeImage *img, unsigned short value, const char *label, int line);
void free_code_image(CodeImage *img);
int init_code_image(CodeImage *img);
int init_name_ref_table(NameRefTable *table);
void free_name_ref_table(NameRefTable *table);
int add_name_ref(NameRefTable *table, const char *name, int line);
void free_label_table(LabelTable *table);
#endif //ASSEMBLERPROJECT_TABLES_H