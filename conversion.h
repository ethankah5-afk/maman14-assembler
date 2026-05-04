//
// Created by ethan on 30/04/2026.
//
#ifndef ASSEMBLERPROJECT_CONVERSION_H
#define ASSEMBLERPROJECT_CONVERSION_H

unsigned short regToShort(char *reg_before);
void toBinary12(unsigned short value, char *binary);
char *find_first_data_number(char *line);

#endif //ASSEMBLERPROJECT_CONVERSION_H
