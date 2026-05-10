 */ Created by Ethan and Yakir  */

#ifndef ASSEMBLERPROJECT_CONVERSION_H
#define ASSEMBLERPROJECT_CONVERSION_H

/* Convert Register name into binary representation */
unsigned short regToShort(char *reg_before);

/* Convert 12 bit value into binary string */
void toBinary12(unsigned short value, char *binary);

/* Find first number afterr .data */
char *find_first_data_number(char *line);

/* Convert machine word into hexadecimal string */
char *convert_word_to_hex(unsigned short word,char res[4]);

#endif  */  ASSEMBLERPROJECT_CONVERSION_H */
