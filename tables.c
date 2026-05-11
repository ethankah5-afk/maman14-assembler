/* 
* Created by Ethan and Yakir
*/

#include <stdlib.h>
#include <string.h>
#include "main_struct.h"
#include "tables.h"

/*
* Initialize labels table 
* table - labels table 
* return - 1 if success otherwise 0 
*/
int initLabelTable(LabelTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (Label *)malloc(table->capacity * sizeof(Label));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}
/*
* Find label by name 
* table - labels table
* name - label name
* return - label index if found otherwise -1 
*/

int findLabel(LabelTable *table,char *name) {
    int i;
    if (table==NULL||name==NULL) {
        return -1;
    }
    /* Search labels table */
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->arr[i].symbol_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/*
* Add new label to labels table 
* table - labels table 
* name - label name 
* address - label address 
* is_data - data label flag 
* line_defined - definition line number 
* return - 1 if success otherwise 0 
*/

int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined) {
    Label *temp;
    
    /* Prevent duplicate labels */
    if (findLabel(table, name) != -1) {
        return 0;
    }
    /* Increase table size if is needed  */
    if (table->count == table->capacity) {
        table->capacity *= 2;
        temp = (Label *)realloc(table->arr, table->capacity * sizeof(Label));
        if (temp == NULL) {
            return 0;
        }
        table->arr = temp;
    } 

    /* label length limit */
    if (strlen(name)>31) {
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

/* 
* Free labels table memory
* table - labels table
*/
void free_label_table(LabelTable *table) {
    if (table->arr != NULL) {
        free(table->arr);
    }

    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}

/* 
* Initialize code image 
* img - code image 
* return - 1 if success otherwise 0 
*/
int init_code_image(CodeImage *img) {
    img->count = 0;
    img->capacity = 10;
    img->arr = (CodeWord *)malloc(img->capacity * sizeof(CodeWord));
    if (img->arr == NULL) {
        return 0;
    }
    return 1;
}

/* 
* Add code word to to code image
* img - code image 
* value - machine word value 
* label - unresolved label 
* line - assembly line number 
* return - 1 if success otherwise 0 
*/
int add_code_word(CodeImage *img, unsigned short value, const char *label, int line) {
    CodeWord *temp;

    /* add to code image size if needed */
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

/*
* Free code image memory 
* img - code image
*/
void free_code_image(CodeImage *img) {
    int i;
    
    /* free all label strings */
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

/* 
* Initialize name references table
* table -  name references table
* return - 1 success otherwise 0 
*/
int  init_name_ref_table(NameRefTable *table) {
    table->count = 0;
    table->capacity = 10;
    table->arr = (NameRef *)malloc(table->capacity * sizeof(NameRef));
    if (table->arr == NULL) {
        return 0;
    }
    return 1;
}
/*  
* Add name to reference table
* table - name references table
* name - referenced name 
*line - assembly line number
* return -  1 success otherwise 0 
 */
int add_name_ref(NameRefTable *table, const char *name, int line) {
    NameRef *temp;

     */ increase table size if is needed */
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

/*
* Free name references table memory 
* table - name references table
*/
void free_name_ref_table(NameRefTable *table) {
    int i;

    /* Free all referenced names */
    for (i = 0; i < table->count; i++) {
        free(table->arr[i].name);
    }
    free(table->arr);
    table->arr = NULL;
    table->count = 0;
    table->capacity = 0;
}
