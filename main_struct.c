#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "main_struct.h"
#define REG_COUNT 8
#define INSTRUCTIONS_COUNT 16
sortMethod sort_methods[]= {
    {'#',0,"immediate"},
    //{.,1,"direct"},

    {'%',2,"relative"},
    {'r',3,"register"}
};
Instruction instructionTable[] = {
   {"mov",  0,  0, 2},
   {"cmp",  1,  0, 2},
   {"add",  2, 10, 2},
   {"sub",  2, 11, 2},
   {"lea",  4,  0, 2},
   {"clr",  5, 10, 1},
   {"not",  5, 11, 1},
   {"inc",  5, 12, 1},
   {"dec",  5, 13, 1},
   {"jmp",  9, 10, 1},
   {"bne",  9, 11, 1},
   {"jsr",  9, 12, 1},
   {"red", 12,  0, 1},
   {"prn", 13,  0, 1},
   {"rts", 14,  0, 0},
   {"stop",15,  0, 0}
};
char *REGS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *DIRECTIVES[] = {".data", ".string", ".extern", ".entry"};

Instruction* findInstruction(const char *name)
{
   int i;
   for (i = 0; i <INSTRUCTIONS_COUNT; i++) {
       if (strcmp(instructionTable[i].name, name) == 0) {
           return &instructionTable[i];
       }
   }
   return NULL;
}
int findReg(char *str) {
    int i;

    /* Return -1 if the string is NULL */
    if (str == NULL) {
        return -1;
    }

    /* Iterate through the list of known registers and compare the string with each register in the list */
    for (i = 0; i < REG_COUNT; i++) {
        if (strcmp(str, REGS[i]) == 0) {
            return i; /* Return the index of the matching register */
        }
    }
    return -1; /* Return -1 if the string does not match any known registers */
}
