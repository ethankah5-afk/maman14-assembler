  /* 
  * Created by Ethan and Yakir 
  */

#ifndef ASSEMBLERPROJECT_ENCODER_H
#define ASSEMBLERPROJECT_ENCODER_H

#include "main_struct.h"

/* 
  * Get ARE for code word 
  * word - code word 
  * externs - extern labels table 
  *return - ARE character 
  */
char get_are_char(CodeWord *word, NameRefTable *externs);

unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count);
unsigned short encode_immediate(char *op);
int handle_instruction_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC,macro_node *macro_table,int macro_count);
int handle_one_operand(char *op,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC);
int is_legal_addressing(Instruction *inst, int src_type, int dest_type, int op_count);
int is_one_of(int type, int a, int b, int c, int d);
unsigned short encode_register(char *op);

/*
* Get operand addressing type 
* op - operand string 
* return - operand addressing type 
*/
int get_addressing_type(char *op);
#endif    /* ASSEMBLERPROJECT_ENCODER_H */
