//
// Created by ethan on 09/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef ASSEMBLERPROJECT_PREPROC_H
#define ASSEMBLERPROJECT_PREPROC_H

typedef __fpos_t fpos_t;
int skip_spaces(char *line,int i) {
    while (line[i]==' '||line[i]=='\t') {
        i++;
    }
    return i;
}
int get_next_word(char *line,int start, char *word) {
    int i;
    int j;
    i=skip_spaces(line,start);
    j=0;
    while (line[i] != '\0' &&
           line[i] != ' ' &&
           line[i] != '\t' &&
           line[i] != '\n' &&
           line[i] != ',') {
        word[j]=line[i];
        j++;
        i++;
    }
    word[j]='\0';
    return i;
}


void *handle_malloc(long object_size) {
    void *object_ptr = malloc(object_size);
    if (object_ptr == NULL) {
        /*print_internal_error(ERROR_CODE_1);*/
    }
    return object_ptr;
}
/*notes:
 *nodes in macroTable
 *dynamic bynding function
 *notice that we need to put the code page first and then the data page
 *dc ic
 *data structures: look in claude
 *
*important:int inc_mem(code_conv **code, int counter) {
code_conv *ptr;
ptr = *code;
*code = realloc(*code, (counter + 1) * sizeof(code_conv));
if (*code == NULL) {
print_internal_error(ERROR_CODE_1);
free(ptr);
return 0;
}
return 1;
}
*/
/* אתחול */
int init_symbol_table(SymbolTableManager *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = malloc(table->capacity * sizeof(symbolTable));
    if (table->arr == NULL) return 0;
    return 1;
}

/* הוספה */
int add_symbol(SymbolTableManager *table, char *name, int address, char *attributes, int is_data) {
    symbolTable *temp;

    /* הגדל אם צריך */
    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = realloc(table->arr, table->capacity * sizeof(symbolTable));
        if (temp == NULL) return 0;
        table->arr = temp;
    }

    /* הוסף את הסימבול */
    table->arr[table->count].symbol_name = malloc(strlen(name) + 1);
    strcpy(table->arr[table->count].symbol_name, name);
    table->arr[table->count].value_address = address;
    table->arr[table->count].attributes = malloc(strlen(attributes) + 1);
    strcpy(table->arr[table->count].attributes, attributes);
    table->arr[table->count].is_data = is_data;

    table->count++;
    return 1;
}
typedef struct {
    symbolTable *arr;
    int count;
    int capacity;
} SymbolTableManager;
#endif //ASSEMBLERPROJECT_PREPROC_H