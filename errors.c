//
// Created by ethan on 04/05/2026.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "Errors.h"

/* The Following array specifies the error code numbers and the corresponding error message.
 * Missing lines are intentionally left blank for future error codes
 * */
Error errors[] = {
        {ERROR_0,  "No Error"},
        {ERROR_1,  "Failed to dynamically allocate memory"},
        {ERROR_2,  ".as files name is missing"},
        {ERROR_3,  "The file name is too long"},
        {ERROR_4,  "The provided file name is not exist"},
        {ERROR_5,  ".am file is missing"},
        {ERROR_6,  "The Line is too long"},
        {ERROR_7,  "Failed to open new file for writing"},
        {ERROR_8,  "Failed to open file for reading"},
        {ERROR_9,  "Error - macro without name definition"},
        {ERROR_10, "Error - extra text after macro name definition"},
        {ERROR_11, "Setting a position in a file failed"},
        {ERROR_12, "endmcro with extra text"},
        {ERROR_13, "Macro has more than one definition"},
        {ERROR_14, "Failed to copy file during macros expansion"},
        {ERROR_15, "Macros expansion in an .as file failed"},
        {ERROR_16, "Macro call before declaration"},
        {ERROR_17, "Illegal name for a macro"},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {ERROR_30, "Line is too long"} ,
        {ERROR_31, "Illegal opcode"},
        {ERROR_32, "Line contains extra text"},
        {ERROR_33, "Illegal argument"},
        {ERROR_34, "Missing argument"},
        {ERROR_35, "Missing comma between argument in a command line with two arguments"},
        {ERROR_36, "Label not defined in the assembly file"},
        {ERROR_37, "Illegal label after .entry"},
        {ERROR_38, "Illegal comma near opcode"},
        {ERROR_39, "More commas than needed"},
        {ERROR_40, "Comma in the wrong place"},
        {ERROR_41, "Illegal char near opcode or label"},
        {ERROR_42, "Illegal char"},
        {ERROR_43, "Missing comma between numbers"},
        {ERROR_44, "Illegal label declaration"},
        {ERROR_45, "Missing ':' after label declaration"},
        {ERROR_46, "Illegal register name. Use only @r1-@r7"},
        {ERROR_47, "Illegal comma before opcode"},
        {ERROR_48, ""},
        {ERROR_49, "Data line without '. before directive"},
        {ERROR_50, "Instruction '.data' line contains non-number info"},
        {ERROR_51, "Comma after the last number in a '.data' line"},
        {ERROR_52, "Missing '\"' after '.string'"},
        {ERROR_53, "Extra text after the string end in '.string' line"},
        {ERROR_54, "IC too big for word CPU word length"},
        {ERROR_55, "Label definition repeats more than once"},
        {ERROR_56, "Label defined as .extern and defined in file"},
        {ERROR_57, "Input number in .data line is out of range"},
        {ERROR_58, "Illegal data line directive"},
        {ERROR_59, "Instruction '.data' line contains illegal chars or syntax error"}

};

void print_internal_error(int ERROR) {
    /* Print the error code number and the error message */
    printf("~~ERROR: ID:%d~~ | %s\n", ERROR, errors[ERROR].error_msg);
}

void print_external_error(int ERROR, location file) {
    /* Print the error code number, file name, assembly line number and the error message */
    printf("~~ERROR: ID:%d~~ in %s at line:%d | there is error: %s\n", ERROR, \
    file.file_name, file.line_num, errors[ERROR].error_msg);
}

//