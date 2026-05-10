//
// Created by ethan on 08/05/2026.
//
#include <stdlib.h>
#include <string.h>
#include "main_struct.h"
#include "tables.h"
int initLabelTable(LabelTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (Label *)malloc(table->capacity * sizeof(Label));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}

int findLabel(LabelTable *table,char *name) {
    int i;
    if (table==NULL||name==NULL) {
        return -1;
    }
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->arr[i].symbol_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined) {
    Label *temp;
    if (findLabel(table, name) != -1) {
        return 0;
    }
    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (Label *)realloc(table->arr, table->capacity * sizeof(Label));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    }
    if (strlen(name)>30) {
        return 0;
    }
    strcpy(table->arr[table->count].symbol_name, name);
    table->arr[table->count].address = address;
    table->arr[table->count].line_defined = line_defined;
    table->arr[table->count].is_data = is_data;
    table->arr[table->count].is_entry = 0;
    table->arr[table->count].is_extern = 0;
    table->count++;
    return 1;
}
void free_label_table(LabelTable *table) {
    if (table->arr != NULL) {
        free(table->arr);
    }

    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}


int init_code_image(CodeImage *img) {
    img->count = 0;
    img->capacity = 10;
    img->arr = (CodeWord *)malloc(img->capacity * sizeof(CodeWord));
    if (img->arr == NULL) {
        return 0;
    }
    return 1;
}
int add_code_word(CodeImage *img, unsigned short value, const char *label, int line) {
    CodeWord *temp;
    if (img->count == img->capacity) {
        img->capacity *= 2;
        temp = (CodeWord *)realloc(img->arr, img->capacity * sizeof(CodeWord));
        if (temp == NULL) {
            return 0;
        }
        img->arr = temp;
    }
    img->arr[img->count].value = value;
    img->arr[img->count].assembly_line = line;
    if (label != NULL) {
        img->arr[img->count].label = (char *)malloc(strlen(label) + 1);
        if (img->arr[img->count].label == NULL) {
            img->arr[img->count].value = 0;
            img->arr[img->count].assembly_line = 0;
            return 0;
        }
        strcpy(img->arr[img->count].label, label);
    }
    else {
        img->arr[img->count].label = NULL;
    }
    img->count++;
    return 1;
}
void free_code_image(CodeImage *img) {
    int i;

    for (i = 0; i < img->count; i++) {
        if (img->arr[i].label != NULL) {
            free(img->arr[i].label);
        }
    }

    free(img->arr);
    img->arr = NULL;
    img->count = 0;
    img->capacity = 0;
}


int  init_name_ref_table(NameRefTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (NameRef *)malloc(table->capacity * sizeof(NameRef));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}
int add_name_ref(NameRefTable *table, const char *name, int line) {
    NameRef *temp;

    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (NameRef *)realloc(table->arr, table->capacity * sizeof(NameRef));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    }
    table->arr[table->count].name = (char *)malloc(strlen(name) + 1);
    if (table->arr[table->count].name == NULL) {
        return 0;
    }
    strcpy(table->arr[table->count].name, name);
    table->arr[table->count].assembly_line = line;
    table->count++;
    return 1;
}
void free_name_ref_table(NameRefTable *table) {
    int i;
    for (i = 0; i < table->count; i++) {
        free(table->arr[i].name);
    }
    free(table->arr);
    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}
