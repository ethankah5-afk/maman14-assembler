/* 
* Created by Ethan and Yakir 
*/
#include <string.h>
#include "encoder.h"
#include "constants.h"
#include "tables.h"
#include "errors.h"
#include "parser.h"
#include "sec_pass.h"

/*
* Get operand addressing type 
* op - operand string 
* return - operand addressing type 
*/

int get_addressing_type(char *op) {
    
    if (op == NULL || op[0] == '\0') {
        return ADDR_INVALID;
    }
     /* Immediate operands start with # */
    if (op[0] == '#') {
        int num;
        if (!is_valid_number(op+1,&num)) {
            return ADDR_INVALID;
        }
        return ADDR_IMMEDIATE;
    }
    /* Relative operands start with % */
    if (op[0] == '%') {
        if (is_label_operands(op + 1)) {
            return ADDR_RELATIVE;
        }
        return ADDR_INVALID;
    }
    /* Register addressing */
    if (findReg(op) != -1) {
        return ADDR_REGISTER;
    }
      /*  Direct label addressing */
    if (is_label_operands(op)) {
        return ADDR_DIRECT;
    }
    return ADDR_INVALID;
}

  /* 
  * Get ARE for code word 
  * word - code word 
  * externs - extern labels table 
  * return - ARE character 
  */
char get_are_char(CodeWord *word, NameRefTable *externs) {
    char *name;
    if (word->label == NULL) {
        return 'A';
    }
    name = word->label;
      /* Remove % before label check */
    if (name[0] == '%'){
            name++;
    }
    if (is_extern_name(externs,name)){
        return 'E';
    }
    return 'R';
}

  /* 
  * Build first instruction word 
  * inst - instruction information 
  * op1 - source operand 
  * op2 - destination operand 
  * op_count - number of operands 
  * return - encoded first word 
  */
unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count) {
    unsigned short word = 0;
    /* Source and destination addressing types */
    int src_type=0;
    int dest_type=0;
    
    if (op_count==2) {
        src_type=get_addressing_type(op1);
        dest_type=get_addressing_type(op2);
    }
    else if (op_count==1) {
        src_type=0;
        dest_type=get_addressing_type(op1);
    }
    else {
        src_type=0;
        dest_type=0;
    }
  /* Encode opcode bits 8-11  */ 
    word |= ((unsigned short)inst->opcode << 8);
    
/* Encode funct into bits 4-7  */
    word |= ((unsigned short)inst->funct << 4);
    
 /* Encode source operand type  */
    word |= ((unsigned short)src_type << 2);
    
 /* Encode destination operand type  */
    word |= (unsigned short)dest_type;
    
    return word;
}
 /* 
 * Encode immediate operand 
 * op - operand string 
 * return - encoded immediate value 
 */
unsigned short encode_immediate(char *op) {
    int num;
    if (!is_valid_number(op + 1, &num)) {
        return 0;
    }
    /* Keep only the lower 12 bits */
    return (unsigned short)(num & 0x0FFF);
}
 /* 
 * Encode register operand 
 * op - register operand 
 * return - encoded register value
 */
unsigned short encode_register(char *op) {
    int reg_num;
    reg_num=findReg(op);
    if (reg_num<0){
        return 0;
    }
     /* Encode Register using the matching bit */
    return (unsigned short)(1<<reg_num);
}


/* 
* Handle operand encoding 
* op - operand string 
* line_num - current line number 
* file_name - source file name 
* labels - labels table 
* code_img - code image 
* IC - instruction counter  
 */

int handle_one_operand(char *op,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC) {
    int operand_address;
    int type;
    int idx;
    unsigned short value;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    operand_address= *IC;
    type = get_addressing_type(op);
    
    /* Handle immediate operand  */
    if (type == ADDR_IMMEDIATE) {
        value=encode_immediate(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
        (*IC)++;
        return 1;
    }
    /* Handle register operand */
    if (type == ADDR_REGISTER) {
        value=encode_register(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
        (*IC)++;
        return 1;
    } 
    /* Handle direct label operand */
    if (type == ADDR_DIRECT) { 
        idx = findLabel(labels, op);
        
        /* Label already resolved in the first pass */
        if (idx != -1 &&
            labels->arr[idx].is_data == 0 &&
            labels->arr[idx].is_extern == 0) {
            if (!add_code_word(code_img,
                               (unsigned short)labels->arr[idx].address,
                               op,
                               line_num)) {
                print_internal_error(ERROR_1);
                return -1;
                               }
            } else {
             /* Label will be resolved in the second pass */
                if (!add_code_word(code_img, 0, op, line_num)) {
                    print_internal_error(ERROR_1);
                    return -1;
                }
            }
        (*IC)++;
        return 1;
    }
     /* Handle relative label operand */
    if (type == ADDR_RELATIVE) {
        char rel_label[31];
        strcpy(rel_label, op + 1);
        idx = findLabel(labels, rel_label);
        if (idx != -1 &&
            labels->arr[idx].is_data == 0 &&
            labels->arr[idx].is_extern == 0) {
            
      /* Calculate relative address */
            if (!add_code_word(code_img,
                               (unsigned short)(labels->arr[idx].address - operand_address),
                               NULL,
                               line_num)) {
                print_internal_error(ERROR_1);
                return -1;}
            } else {
            /* Label will be resolved in the second pass */
                if (!add_code_word(code_img, 0, op, line_num)) {
                    print_internal_error(ERROR_1);
                    return -1;
                }
            }
        (*IC)++;
        return 1;
    }
    print_external_error(ERROR_19, loc);
    return 0;
}
/* 
* Check if value matches one of the given options 
* type - value to check 
* return - if found 1, if not found 0 
 */
int is_one_of(int type, int a, int b, int c, int d) {
    return (type == a || type == b || type == c || type == d);
}

/* 
* Check if instruction addressing is legal 
* inst - instruction format
* src_type - source operand type 
* dest_type - destination operand type 
* return - 1 if legal, if not legal 0
 */

int is_legal_addressing(Instruction *inst, int src_type, int dest_type, int op_count) {
    if (inst == NULL) {
        return 0;
    }
    switch (inst->opcode) {
        case 0: /* mov */
            return op_count == 2 &&
                   is_one_of(src_type, 0, 1, 3, -1) &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 1: /* cmp */
            return op_count == 2 &&
                   is_one_of(src_type, 0, 1, 3, -1) &&
                   is_one_of(dest_type, 0, 1, 3, -1);

        case 2: /* add/sub */
            if (inst->funct == 10 || inst->funct == 11) {
                return op_count == 2 &&
                       is_one_of(src_type, 0, 1, 3, -1) &&
                       is_one_of(dest_type, 1, 3, -1, -1);
            }
            return 0;

        case 4: /* lea */
            return op_count == 2 &&
                   src_type == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 5: /* clr/not/inc/dec */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 9: /* jmp/bne/jsr */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 2, -1, -1);

        case 12: /* red */
            return op_count == 1 &&
                   is_one_of(dest_type, 1, 3, -1, -1);

        case 13: /* prn */
            return op_count == 1 &&
                   is_one_of(dest_type, 0, 1, 3, -1);

        case 14: /* rts */
        case 15: /* stop */
            return op_count == 0;

        default:
            return 0;
    }
}
/*  
* Handle instruction line during the first pass
* line - source line 
* line_num - current line number 
* file_name - file name 
* labels - labels table 
* code_img - code image 
* IC - instruction counter 
* macro_table - macros table 
* macro_count - number of macros 
*/
int handle_instruction_line(char *line,int line_num,char *file_name,LabelTable *labels,CodeImage *code_img,int *IC,macro_node *macro_table,int macro_count){
    char temp[MAX_LINE_LENGTH];
    char *token;
    char label_name[31];
    char *operands_line;
    char op1[MAX_LINE_LENGTH];
    char op2[MAX_LINE_LENGTH];
    int has_label;
    int op_count;
    int src_type;
    int dest_type;
    Instruction *inst;
    unsigned short first_word;
    int status;
    location loc;
    loc.file_name = file_name;
    loc.line_num = line_num;
    has_label =0;
    op_count =0;
    strcpy(temp,line);
    token=strtok(temp," \t\n");
    if (token == NULL) {
        return 0;
    }
    /* check if line starts with label */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,labels,macro_table,macro_count)) {
             print_external_error(ERROR_28, loc);
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            print_external_error(ERROR_17, loc);
            return 0;
        }
    }
    inst=findInstruction(token);
    if (inst==NULL) {
          print_external_error(ERROR_17, loc);
        return 0;
    }

    if (has_label && findLabel(labels, label_name) != -1) {
        print_external_error(ERROR_35, loc);
        return 0;
    }
    if (has_label){
        if (!addLabel(labels,label_name,*IC,0,line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
    }
    operands_line = strtok(NULL, "\n");
    op1[0] = '\0';
    op2[0] = '\0';
    
     /*  Validate comma placement before parsing  */
    if (operands_line != NULL) {
        if (strstr(operands_line, ",,") != NULL) {
            print_external_error(ERROR_24, loc);
            return 0;
        }
        if (operands_line[0] == ',') {
            print_external_error(ERROR_25, loc);
            return 0;
        }
        if (operands_line[strlen(operands_line) - 1] == ',') {
            print_external_error(ERROR_18, loc);
            return 0;
        }
    }
    if (operands_line!=NULL&&inst->operand_count==2&&strchr(operands_line,',')==NULL) {
        print_external_error(ERROR_21,loc);
        return 0;
    }
     /* Parse operands from instruction line */ 
    parse_operands(operands_line,op1,op2,&op_count);
    
    if (op_count==-1) {
        print_external_error(ERROR_18, loc);
        return 0;
    }
    if (op_count!=inst->operand_count) {
        print_external_error(ERROR_20, loc);
        return 0;
    }
     /* Instruction with source and destination operands */ 
    if (op_count ==2) {
        src_type = get_addressing_type(op1);
        dest_type = get_addressing_type(op2);
        if (src_type == ADDR_INVALID || dest_type ==ADDR_INVALID) {
            print_external_error(ERROR_19, loc);
            return 0;
        }
        if (!is_legal_addressing(inst, src_type, dest_type, op_count)) {
             print_external_error(ERROR_19, loc);
            return 0;
        }
    }
    /* Instruction with single operand */
    else if (op_count ==1) {
        dest_type = get_addressing_type(op1);
        if (dest_type == ADDR_INVALID) {
            print_external_error(ERROR_19, loc);
            return 0;
        }
        if (!is_legal_addressing(inst, 0, dest_type, op_count)) {
             print_external_error(ERROR_19, loc);
            return 0;
        }
    }
      /* Instruction without operands */     
    else{
        if (!is_legal_addressing(inst,0,0,op_count)) {
             print_external_error(ERROR_19, loc);
            return 0;
        }
    }
    
    /* Encode first instruction word */
    first_word=build_first_word(inst,op1,op2,op_count);
    if (!add_code_word(code_img,first_word,NULL,line_num)){
        print_internal_error(ERROR_1);
        return -1;
    }
    (*IC)++;

    if (op_count==0) {
        return 1;
    }
     /* Encode first operand */
    if (op_count==1) {
        return handle_one_operand(op1,line_num,file_name,labels,code_img,IC);
    }
     /* Encode second operand */
    if(op_count==2) {
        status=handle_one_operand(op1,line_num,file_name,labels,code_img,IC);
        if (status==-1) {
            return -1;
        }
        if (status==0) {
            return 0;
        }
        status=handle_one_operand(op2,line_num,file_name,labels,code_img,IC);
        if (status==-1) {
            return -1;
        }
        if (status==0) {
            return 0;
        }
        return 1;
    }
    return 0;
}
