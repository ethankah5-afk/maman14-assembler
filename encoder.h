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

 /* 
  * Build first instruction word 
  * inst - instruction information 
  * op1 - source operand 
  * op2 - destination operand 
  * op_count - number of operands 
  * return - encoded first word 
  */
unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count);

 /* 
 * Encode immediate operand 
 * op - operand string 
 * return - encoded immediate value 
 */
unsigned short encode_immediate(char *op);


int handle_instruction_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC,macro_node *macro_table,int macro_count);

/* 
* Handle operand encoding 
* op - operand string 
* line_num - current line number 
* file_name - source file name 
* labels - labels table 
* code_img - code image 
* IC - instruction counter  
 */
int handle_one_operand(char *op,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC);

/* 
* Check if instruction addressing is legal 
* inst - instruction format
* src_type - source operand type 
* dest_type - destination operand type 
* return - 1 if legal, if not legal 0
 */
int is_legal_addressing(Instruction *inst, int src_type, int dest_type, int op_count);

/* 
* Check if value matches one of the given options 
* type - value to check 
* return - if found 1, if not found 0 
 */
int is_one_of(int type, int a, int b, int c, int d);


/* 
 * Encode register operand 
 * op - register operand 
 * return - encoded register value
 */
unsigned short encode_register(char *op);

/*
* Get operand addressing type 
* op - operand string 
* return - operand addressing type 
*/
int get_addressing_type(char *op);
#endif    /* ASSEMBLERPROJECT_ENCODER_H */
