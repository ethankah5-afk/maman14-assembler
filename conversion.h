 */ Created by Ethan and Yakir  */

#ifndef ASSEMBLERPROJECT_CONVERSION_H
#define ASSEMBLERPROJECT_CONVERSION_H

/* 
* Convert register name to its binary representaion
* reg_before - register name 
* return - register binary value 
*/
unsigned short regToShort(char *reg_before);

/* 
* Convert a 12 bit value into a binary string
* value - number to convert 
* binary - output
*/

void toBinary12(unsigned short value, char *binary);

/* 
* find the first number after .data
* line - source line 
* return - pointer to the frist data number 
*/
char *find_first_data_number(char *line);

/* 
* Convert a machine word into a hexadecimal string
* word - machine to to convert 
* res - output hexadecimal string 
* return - pointer to result 
*/
char *convert_word_to_hex(unsigned short word,char res[4]);

#endif  */  ASSEMBLERPROJECT_CONVERSION_H */
