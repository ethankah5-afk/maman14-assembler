/* 
* Created by Ethan and Yakir 
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "main_struct.h"
#include "conversion.h"

/* 
* Convert register name to its binary representation
* reg_before - register name 
* return - register binary value 
*/
unsigned short regToShort(char *reg_before) {
    int num_of_reg=findReg(reg_before);
    
    /* not Valid register number */
    if (num_of_reg<0) {
        return 0;
    }
    /* Turn on the matching bit for the register number  */
    return (unsigned short)(1<<num_of_reg);
}

/* 
* Convert a 12 bit value into a binary string
* value - number to convert 
* binary - output
*/

void toBinary12(unsigned short value, char *binary){
    int i;
    
    /* start from the Leftmost bit */
    unsigned short mask = 1 << 11;
    for (i = 0; i < 12; i++) {
        binary[i] = (value & mask) ? '1' : '0';
        
        /* move mask one bit to the right  */
        mask >>= 1;
    }
    binary[12] = '\0';
}
/* 
* find the first number after .data
* line - source line 
* return - pointer to the first data number 
*/
char *find_first_data_number(char *line){
    char *p = strstr(line, ".data");
    if (p == NULL) {
        return NULL;
    }
    
    /* skip spaces after .data */
    p += 5;
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}
/* 
* Convert a machine word into a hexadecimal string
* word - machine to convert 
* res - output hexadecimal string 
* return - pointer to result 
*/

char *convert_word_to_hex(unsigned short word,char res[4]) {
    char hex_table[] = "0123456789ABCDEF";
    
    /* Extract first hex digit */
    res[0] = hex_table[(word >> 8) & 0xF];
    
    /* Extract second hex digit */
    res[1] = hex_table[(word >> 4) & 0xF];
    
    /* Extract last hex digit */
    res[2] = hex_table[word & 0xF];
    
    res[3] = '\0';
    return res;
}
