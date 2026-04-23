//
// Created by ethan on 08/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "main_struct.h"
#define MAX_LINE 81
unsigned short regToShort(char *reg_before) {
    int num_of_reg=findReg(reg_before);
    if (num_of_reg<0) {
        return 0;
    }
    return (unsigned short)(1<<num_of_reg);
}
void toBinary12(unsigned short value, char *binary)
{
    int i;
    unsigned short mask = 1 << 11;
    for (i = 0; i < 12; i++) {
        binary[i] = (value & mask) ? '1' : '0';
        mask >>= 1;
    }
    binary[12] = '\0';
}

char *find_first_data_number(char *line)
{
    char *p = strstr(line, ".data");
    if (p == NULL) {
        return NULL;
    }
    p += 5;
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}
