/*
* Created by Ethan and Yakir
*/ 
#ifndef ASSEMBLERPROJECT_MAIN_STRUCT_H
#define ASSEMBLERPROJECT_MAIN_STRUCT_H

/* 
* Find register index by name
* str - register name 
* return - register index if found, if not found -1
*/
int findReg(char *str);

/* stores file name and line number */
typedef struct location {
    char *file_name;
    int line_num;
} location;

/* Represents instruction information  */
typedef struct Instruction{
    char *name;
    int opcode;
    int funct;
    int operand_count;
}Instruction;

/* Represents addressing method information */
typedef struct sortMethod {
    char method_symbol;
    int num;
    char *name;
}sortMethod;

/* Represents a Label in the assembler */
typedef struct Label{
    char symbol_name[31];
    int address;
    int line_defined;
    int is_data;
    int is_entry;
    int is_extern;
}Label;

/* Dynamic table of labels  */
typedef struct LabelTable{
    Label *arr;
    int count;
    int capacity;
} LabelTable;

/* Output Table line */
typedef struct generalTable {
    int address;
    char *source_code;
    char *binary_code;
    char ARE;
}generalTable;

/*  Represents macro information */
typedef struct macro_node {
    char *name;
    char *content;
}macro_node;

/* Represents encoded machine word */
typedef struct CodeWord {
    unsigned short value;
    char *label;
    int assembly_line;
} CodeWord;

/* Dynamic machine code image */
typedef struct CodeImage {
    CodeWord *arr;
    int count;
    int capacity;
} CodeImage;

/* Represents entry or extern label refernce */
typedef struct NameRef {
    char *name;
    int assembly_line;
} NameRef;

/* Dynamic table of entry or extern label refernces  */
typedef struct NameRefTable {
    NameRef *arr;
    int count;
    int capacity;
} NameRefTable;

/* 
* Find instruction by name
* name - instruction name
* return - pointer to instruction if found, if not NULL 
*/
Instruction* findInstruction(const char *name);

#endif  /* ASSEMBLERPROJECT_MAIN_STRUCT_H */ 
