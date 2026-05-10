//
// Created by ethan on 04/05/2026.
//

#ifndef ASSEMBLERPROJECT_ERRORS_H
#define ASSEMBLERPROJECT_ERRORS_H

#ifndef LABRATORY_C_FINAL_PROJECT_ERRORS_H
#define LABRATORY_C_FINAL_PROJECT_ERRORS_H

#include "main_struct.h"

/* Represents an error with an associated error ID and error message */
typedef struct Error {
    int error_id;     /* Unique identifier for the error */
    char *error_msg;  /* Error message associated with the error */
} Error;

/* Represents error codes for various types of errors */
typedef enum ERROR_CODES {
    /* The following errors are macro related or general */
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

/**
 * @brief Prints an external error message.
 *
 * This function prints to stdout an error that is related to an error in the source file
 * with error number and the error location in the source file
 * @param error_code error number
 * @param file a pointer to a struct which contains the source file name and line information
 */
void print_external_error(int error_code, location file);

/**
 * @brief Prints an internal error message.
 *
 * This function prints to stdout an error that is a result of an internal process and NOT an error in the source file
 * @param error_code error number
 */
void print_internal_error(int error_code);

#endif

#endif //ASSEMBLERPROJECT_ERRORS_H
