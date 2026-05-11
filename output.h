/* 
* Created by Ethan and Yakir 
*/

#ifndef ASSEMBLERPROJECT_OUTPUT_H
#define ASSEMBLERPROJECT_OUTPUT_H
#include "main_struct.h"

/*
* Build output file name with extension 
* file_name- source file name 
* ext - output extension 
* out - output file namw
*/
void make_output_name(char *file_name, char *ext, char *out);

/*
* Write .ent output file 
* file_name - source file name 
* labels - labels table 
* return - 1 if success if not 0 
*/
int write_ent_file(char *file_name,LabelTable *labels);

/*
* Write .ext output file 
* file_name - source file name 
* code_img - code image 
* externs - extern labels table 
* return - 1 if success if not 0 
*/
int write_ext_file(char *file_name, CodeImage *code_img,NameRefTable *externs);

/* 
* Write .ob output file 
* file_name - source file name 
* code_img - code image 
* data_img - data image 
* externs - extern labels table 
* IC - final instruction counter
* DC -  final data counter 
* return - 1 if succes if not 0
*/
int write_ob_file(char *file_name,CodeImage *code_img,CodeImage *data_img,NameRefTable *externs,int IC, int DC);

#endif  /* ASSEMBLERPROJECT_OUTPUT_H */
