/* 
* Created by Ethan and Yakir
*/

#ifndef ASSEMBLERPROJECT_TABLES_H
#define ASSEMBLERPROJECT_TABLES_H
#include "main_struct.h"

/*
* Initialize labels table 
* table - labels table 
* return - 1 if success otherwise 0 
*/
int initLabelTable(LabelTable *table);

/*
* Find label by name 
* table - labels table
* name - label name
* return - label index if found otherwise -1 
*/
int findLabel(LabelTable *table, char *name);

/*
* Add new label to labels table 
* table - labels table 
* name - label name 
* address - label address 
* is_data - data label flag 
* line_defined - definition line number 
* return - 1 if success otherwise 0 
*/
int addLabel(LabelTable *table, char *name, int address, int is_data, int line_defined);

/* 
* Add code word to code image
* img - code image 
* value - machine word value 
* label - unresolved label 
* line - assembly line number 
* return - 1 if success otherwise 0 
*/
int add_code_word(CodeImage *img, unsigned short value, const char *label, int line);


/*
* Free code image memory 
* img - code image
*/
void free_code_image(CodeImage *img);

/* 
* Initialize code image 
* img - code image 
* return - 1 if success otherwise 0 
*/
int init_code_image(CodeImage *img);



/* 
* Initialize name references table
* table -  name references table
* return - 1 if success otherwise 0 
*/
int init_name_ref_table(NameRefTable *table);


/*
* Free name references table memory 
* table - name references table
*/
void free_name_ref_table(NameRefTable *table);

/*  
* Add name to reference table
* table - name references table
* name - referenced name 
* line - assembly line number
* return -  1 if success otherwise 0 
 */
int add_name_ref(NameRefTable *table, const char *name, int line);


/* 
* Free labels table memory
* table - labels table
*/
void free_label_table(LabelTable *table);


#endif /*  ASSEMBLERPROJECT_TABLES_H  */
