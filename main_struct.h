//
// Created by ethan on 08/03/2026.
//

#ifndef ASSEMBLERPROJECT_MAIN_STRUCT_H
#define ASSEMBLERPROJECT_MAIN_STRUCT_H
int findReg(char *str);
typedef struct Instruction{
    char *name;
    int opcode;
    int funct;
    int operand_count;
}Instruction;

typedef struct sortMethod {
    char method_symbol;
    int num;
    char *name;
}sortMethod;

typedef struct Label{
    char symbol_name[31];
    int address;
    int line_defined;
    int is_data;
    int is_entry;
    int is_extern;
}Label;

typedef struct LabelTable{
    Label *arr;
    int count;
    int capacity;
} LabelTable;

typedef struct generalTable {
    int address;
    char *source_code;
    char *binary_code;
    char ARE;
}generalTable;

typedef struct macro_node {
    char *name;
    char *content;
}macro_node;

typedef struct CodeWord {
    unsigned short value;
    char *label;
    int assembly_line;
} CodeWord;

typedef struct CodeImage {
    CodeWord *arr;
    int count;
    int capacity;
} CodeImage;

typedef struct NameRef {
    char *name;
    int assembly_line;
} NameRef;

typedef struct NameRefTable {
    NameRef *arr;
    int count;
    int capacity;
} NameRefTable;

Instruction* findInstruction(const char *name);

#endif //ASSEMBLERPROJECT_MAIN_STRUCT_H
