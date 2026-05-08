//
// Created by ethan on 08/05/2026.
//
#include <string.h>
#include "encoder.h"
#include "constants.h"
#include "tables.h"
#include "errors.h"
#include "parser.h"

int get_addressing_type(char *op) {
    if (op == NULL || op[0] == '\0') {
        return ADDR_INVALID;
    }
    if (op[0] == '#') {
        int num;
        if (!is_valid_number(op+1,&num)) {
            return ADDR_INVALID;
        }
        return ADDR_IMMEDIATE;
    }
    if (op[0] == '%') {
        if (is_label_operands(op + 1)) {
            return ADDR_RELATIVE;
        }
        return ADDR_INVALID;
    }
    if (findReg(op) != -1) {
        return ADDR_REGISTER;
    }
    if (is_label_operands(op)) {
        return ADDR_DIRECT;
    }
    return ADDR_INVALID;
}

unsigned short build_first_word(Instruction *inst, char *op1, char *op2, int op_count) {
    unsigned short word = 0;
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
    word |= ((unsigned short)inst->opcode << 8);
    word |= ((unsigned short)inst->funct << 4);
    word |= ((unsigned short)src_type << 2);
    word |= (unsigned short)dest_type;
    return word;
}
unsigned short encode_immediate(char *op) {
    int num;
    if (!is_valid_number(op + 1, &num)) {
        return 0;
    }
    return (unsigned short)(num & 0x0FFF);
}

unsigned short encode_register(char *op) {
    int reg_num;
    reg_num=findReg(op);
    if (reg_num<0){
        return 0;
    }
    return (unsigned short)(1<<reg_num);
}

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
    if (type == ADDR_IMMEDIATE) {
        value=encode_immediate(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_REGISTER) {
        value=encode_register(op);
        if (!add_code_word(code_img, value, NULL, line_num)) {
            print_internal_error(ERROR_1);
            return -1;
        }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_DIRECT) {
        idx = findLabel(labels, op);
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
                if (!add_code_word(code_img, 0, op, line_num)) {
                    print_internal_error(ERROR_1);
                    return -1;
                }
            }
        (*IC)++;
        return 1;
    }
    if (type == ADDR_RELATIVE) {
        char rel_label[31];
        strcpy(rel_label, op + 1);
        idx = findLabel(labels, rel_label);
        if (idx != -1 &&
            labels->arr[idx].is_data == 0 &&
            labels->arr[idx].is_extern == 0) {
            if (!add_code_word(code_img,
                               (unsigned short)(labels->arr[idx].address - operand_address),
                               NULL,
                               line_num)) {
                print_internal_error(ERROR_1);
                return -1;}
            } else {
                if (!add_code_word(code_img, 0, op, line_num)) {
                    print_internal_error(ERROR_1);
                    return -1;
                }
            }
        (*IC)++;
        return 1;
    }
    print_external_error(ERROR_33, loc);
    return 0;
}
int is_one_of(int type, int a, int b, int c, int d) {
    return (type == a || type == b || type == c || type == d);
}
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
    /* אם יש label בתחילת שורה */
    if (token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        if (!is_valid_label(token,labels,macro_table,macro_count)) {
             print_external_error(ERROR_44, loc);
            return 0;
        }
        strcpy(label_name, token);
        has_label = 1;
        token = strtok(NULL, " \t\n");
        if (token == NULL) {
            print_external_error(ERROR_31, loc);
            return 0;
        }
    }
    inst=findInstruction(token);
    if (inst==NULL) {
          print_external_error(ERROR_31, loc);
        return 0;
    }

    if (has_label && findLabel(labels, label_name) != -1) {
        print_external_error(ERROR_55, loc);
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
    if (operands_line != NULL) {
        if (strstr(operands_line, ",,") != NULL) {
            print_external_error(ERROR_39, loc);
            return 0;
        }
        if (operands_line[0] == ',') {
            print_external_error(ERROR_40, loc);
            return 0;
        }
        if (operands_line[strlen(operands_line) - 1] == ',') {
            print_external_error(ERROR_47, loc);
            return 0;
        }
    }
    if (operands_line!=NULL&&inst->operand_count==2&&strchr(operands_line,',')==NULL) {
        print_external_error(ERROR_35,loc);
        return 0;
    }
    parse_operands(operands_line,op1,op2,&op_count);
    if (op_count==-1) {
        print_external_error(ERROR_32, loc);
        return 0;
    }
    if (op_count!=inst->operand_count) {
        print_external_error(ERROR_34, loc);
        return 0;
    }

    if (op_count ==2) {
        src_type = get_addressing_type(op1);
        dest_type = get_addressing_type(op2);
        if (src_type == ADDR_INVALID || dest_type ==ADDR_INVALID) {
            print_external_error(ERROR_33, loc);
            return 0;
        }
        if (!is_legal_addressing(inst, src_type, dest_type, op_count)) {
             print_external_error(ERROR_33, loc);
            return 0;
        }
    }
    else if (op_count ==1) {
        dest_type = get_addressing_type(op1);
        if (dest_type == ADDR_INVALID) {
            print_external_error(ERROR_33, loc);
            return 0;
        }
        if (!is_legal_addressing(inst, 0, dest_type, op_count)) {
             print_external_error(ERROR_33, loc);
            return 0;
        }
    }
    else{
        if (!is_legal_addressing(inst,0,0,op_count)) {
             print_external_error(ERROR_33, loc);
            return 0;
        }
    }
    first_word=build_first_word(inst,op1,op2,op_count);
    if (!add_code_word(code_img,first_word,NULL,line_num)){
        print_internal_error(ERROR_1);
        return -1;
    }
    (*IC)++;

    if (op_count==0) {
        return 1;
    }
    if (op_count==1) {
        return handle_one_operand(op1,line_num,file_name,labels,code_img,IC);
    }

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