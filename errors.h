/*
* Created by Ethan and Yakir 
*/

#ifndef ASSEMBLERPROJECT_ERRORS_H
#define ASSEMBLERPROJECT_ERRORS_H

#include "main_struct.h"

/* Represents an error with an associated error ID and error message */
typedef struct Error {
    int error_id;     /* Error code identifier */
    char *error_msg;  /* Error message text */
} Error;

   /* Error codes used across the assembler project  */
typedef enum ERROR_CODES {
    ERROR_0 = 0,
    ERROR_1,
    ERROR_2,
    ERROR_3,
    ERROR_4,
    ERROR_5,
    ERROR_6,
    ERROR_7,
    ERROR_8,
    ERROR_9,
    ERROR_10,
    ERROR_11,
    ERROR_12,
    ERROR_13,
    ERROR_14,
    ERROR_15,
    ERROR_16,
    ERROR_17,
    ERROR_18,
    ERROR_19,
    ERROR_20,
    ERROR_21,
    ERROR_22,
    ERROR_23,
    ERROR_24,
    ERROR_25,
    ERROR_26,
    ERROR_27,
    ERROR_28,
    ERROR_29,
    ERROR_30,
    ERROR_31,
    ERROR_32,
    ERROR_33,
    ERROR_34,
    ERROR_35,
    ERROR_36,
    ERROR_37,
    ERROR_38,
} ERROR_CODES;

/* 
* Print external error with file location  
* error_code- error code number 
* file - file location 
*/
void print_external_error(int error_code, location file);

/* 
* Print internal system error 
* error_code - error code number
*/
void print_internal_error(int error_code);

#endif
