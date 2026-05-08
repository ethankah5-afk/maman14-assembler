//
// Created by ethan on 04/05/2026.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "errors.h"
#include "main_struct.h"

/* The Following array specifies the error code numbers and the corresponding error message.
 * Missing lines are intentionally left blank for future error codes
 * */
Error errors[] = {
        {ERROR_0,  "No Error"},
        {ERROR_1,  "Failed to allocate memory dynamically (also refers to creating files)"},
        {ERROR_2,  ".as file name is missing"},
        {ERROR_3,  "file name is too long"},
        {ERROR_4,  "file does not exist"},
        {ERROR_5,  ".am file is missing"},
        {ERROR_6,  "Line is too long"},
        {ERROR_7,  "Failed to open output file for writing"},
        {ERROR_8,  "Failed to open input file for reading"},
        {ERROR_9,  "Macro definition is missing a macro name"},
        {ERROR_10, "Extra text after macro definition"},
        {ERROR_11, "Failed to set or get position in file"},
        {ERROR_12, "Extra text after mcroend"},
        {ERROR_14, "Failed to copy file during macros expansion"},
        {ERROR_15, "Macros expansion in an .as file failed"},
        {ERROR_16, "Illegal macro name"},
        {ERROR_17, "not valid opcode"},
        {ERROR_32, "Line contains extra text"},//18
        {ERROR_33, "Illegal argument"},//19
        {ERROR_34, "Missing argument"},//20
        {ERROR_35, "Missing comma between operands"},//21
        {ERROR_36, "Undefined label"},//22
        {ERROR_37, "Illegal label after .entry"},//23
        {ERROR_39, "More commas than needed"},//24
        {ERROR_40, "Comma in the wrong place"},//25
        {ERROR_42, "Illegal char"},//26
        {ERROR_43, "Missing comma between numbers"},//27
        {ERROR_44, "Illegal label"},//28
        {ERROR_47, "Illegal comma before opcode"},//29
        {ERROR_49, "Data line without '.' before directive"},//30
        {ERROR_50, "Instruction '.data' line contains non-number info"},//31
        {ERROR_51, "Comma after the last number in a '.data' line"},//32
        {ERROR_52, "Missing quotes in .string"},//33
        {ERROR_53, "Extra text after .string"},//34
        {ERROR_55, "Label definition is written more than once"},//35
        {ERROR_56, "Label defined as .extern and .entry"},//36
        {ERROR_57, "Input number in .data line is out of range"},//37
        {ERROR_58, "Illegal data line directive"},//38
};

char *get_error_msg(int error_code){
    int i;
    int errors_count;
    errors_count = (int)(sizeof(errors) / sizeof(errors[0]));
    for (i = 0; i < errors_count; i++) {
        if (errors[i].error_id == error_code) {
            return errors[i].error_msg;
        }
    }
    return "Unknown error";
}

void print_internal_error(int error_code)
{
    printf("[ERROR %d] %s\n", error_code, get_error_msg(error_code));
}

void print_external_error(int error_code, location file)
{
    printf("[ERROR %d] In %s at line %d: %s\n" ,error_code,file.file_name,file.line_num,get_error_msg(error_code));
}
//
