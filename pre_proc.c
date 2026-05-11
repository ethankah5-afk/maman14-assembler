/*
* Created by Ethan and Yakir 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_struct.h"
#include "pre_proc.h"
#include "errors.h"
#include "parser.h"
#include "constants.h"

/* 
* Free macro table memory
* table - macros table 
* table_size - number of macros 
*/
void free_macro_table(macro_node *table, int table_size) {
    int i;
    /* Free all macro names and contents */
    for (i = 0; i < table_size; i++) {
        free(table[i].name);
        free(table[i].content);
    }
    free(table);
}
/* 
* Build file name with new extension
* file_name - source file name 
* ending - new extension
* return - new file name
*/

char *add_new_file(char *file_name, char *ending) {
    char *dot;
    char *new_file_name;

    new_file_name = (char *)malloc(strlen(file_name) + strlen(ending) + 1);
    if (new_file_name == NULL) {
        return NULL;
    }

    strcpy(new_file_name, file_name);
    
    /* Find current extension   */
    dot = strchr(new_file_name, '.');

    /* Remove old extension */
    if (dot != NULL) {
        *dot = '\0';
    }
    
    /* Add new extension */
    strcat(new_file_name, ending);
    return new_file_name;
}

/* 
* check if line defines a macro
* line_num - source line
* mcro_name - returned macro name
* error_code - returned error code
* return - status code
*/

int is_macro(char *line_num,char **mcro_name,ERROR_CODES *error_code) {
    char *token;
    char *extra;
    token= strtok(line_num," \t\n");
    if (token == NULL) return 0;
    
    /* Line does not start with mcro */
    if (strcmp(token,"mcro")!=0) {
        return 0;
    }
    token= strtok(NULL," \t\n");
    
    /* if Missing macro name */
    if (token == NULL) {
        *error_code=ERROR_9;
        return -1;
    }
    *mcro_name=token;

    /* Detect extra text after macro name */
    extra = strtok(NULL," \t\n");
    if (extra != NULL) {
        *error_code=ERROR_10;
        return -1;
    }
    return 1;
}

/*
* Save macro content from file
* fp - source file 
* pos - position in file
* line_count - current line counter
* error_code - returned error code
* return - macro content
*/
char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count,ERROR_CODES *error_code) {
    char *p;
    int found_end=0;
    int mcro_length=0;
    char *mcro;
    char str[MAX_LINE_LENGTH];
    char str_copy[MAX_LINE_LENGTH];

    /* Set file position to saved macro start  */
    if (fsetpos(fp,pos)!=0) {
        *error_code=ERROR_11;
        return NULL;
    }
    str[0]='\0';
    
    /* Calculate macro content length */
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL) {
        (*line_count)++;
        strcpy(str_copy,str);
        trim_spaces(str_copy);

        /* Detect mcroend line */
        if (strncmp(str_copy,"mcroend",7)==0) {
            p=str_copy+7;
            while (*p==' '||*p=='\t') {
                p++;
            }
            
            /* extra text after mcroend */
            if (*p!='\n'&&*p!='\0') {
                *error_code=ERROR_12;
                return NULL;
            }
            found_end=1;
            break;
        }
        mcro_length+=(int)strlen(str);
    }
    /* missing mcroend */
    if (!found_end) {
        *error_code=ERROR_15;
        return NULL;
    }
    mcro = (char *)malloc(mcro_length+1);
    if (mcro==NULL) {
        *error_code=ERROR_1;
        return NULL;
    }
    /* Return again to macro content start */
    if (fsetpos(fp,pos)!=0) {
        *error_code=ERROR_11;
        free(mcro);
        return NULL;
    }
    mcro[0]= '\0';

    /* Copy macro lines */
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL &&strncmp(str,"mcroend",7)!=0) {
        strcat(mcro,str);
    }
    return mcro;
}

/*
* check if line is macro call 
* line - source line
* macro_name - macro name
* return - 1 if is macro call otherwise 0 
*/
int is_macro_call(char *line, char *macro_name) {
    char copy[MAX_LINE_LENGTH];
    char *token;
    char *extra;
    strcpy(copy, line);
    token = strtok(copy, " \t\n");
    if (token == NULL) {
        return 0;
    }
    /* First token does not match macro */
    if (strcmp(token, macro_name) != 0) {
        return 0;
    }
    
    /*  Macro call cannot contain extra text */
    extra = strtok(NULL, " \t\n");
    if (extra != NULL) {
        return 0;
    }
    return 1;
}

/* 
* Find macro by name
* table - macros table
* table_size - number of macros
* name - macro name 
* return - 1 if found otherwise -1
*/
int find_macro(macro_node *table, int table_size, char *name) {
    int i;
    if(table ==NULL || name == NULL) {
        return -1;
    }
    /* Search macro table */
    for (i =0; i<table_size;i++) {
        if(strcmp(table[i].name, name) == 0) {
            return 1;
        }
    }
    return -1;
}

/*
* Run preprocessor on assembly file
* file_name - source file name
* macro_table - returned macros table 
* macro_count - returned number of macros
* return - 1 if succes if not 0
*/
int run_preproc(char *file_name,macro_node **macro_table,int *macro_count) {
    FILE *fp_in, *fp_out;
    char line[MAX_LINE_LENGTH];
    char line_copy[MAX_LINE_LENGTH];
    char *mcro_name, *am_file, *content;
    fpos_t pos;
    macro_node *table = NULL;
    macro_node *temp;
    int table_size = 0;
    int line_count = 0;
    int macro_status;
    int found;
    int i;
    int error_found=0;
    ERROR_CODES error_code;
    location file_location;
    fp_in = fopen(file_name, "r");
    if (fp_in == NULL) {
        print_internal_error(ERROR_8);
        return 0;
    }
    /* Create .am file name */
    
    am_file = add_new_file(file_name, ".am");
    if (am_file==NULL) {
        print_internal_error(ERROR_1);
        fclose(fp_in);
        return 0;
    }
    fp_out = fopen(am_file, "w");
    if (fp_out == NULL) {
        print_internal_error(ERROR_7);
        fclose(fp_in);
        free(am_file);
        return 0;
    }
    /* Read source file line by line */
    while (fgets(line,MAX_LINE_LENGTH , fp_in)) {
        line_count++;
        file_location.file_name = file_name;
        file_location.line_num = line_count;
        strcpy(line_copy, line);

        /* check if line defines a macro */
        macro_status=is_macro(line_copy,&mcro_name,&error_code);
        if (macro_status==-1) {
            print_external_error(error_code,file_location);
            error_found=1;
            continue;
        }
        /* Handle macro definition */
        if (macro_status==1) {

            /* Validate macro name */
            
            if (findInstruction(mcro_name) != NULL ||
                findReg(mcro_name) != -1 ||
                strcmp(mcro_name, ".data") == 0 ||
                strcmp(mcro_name, ".string") == 0 ||
                strcmp(mcro_name, ".extern") == 0 ||
                strcmp(mcro_name, ".entry") == 0||
                strlen(mcro_name)>31){
                print_external_error(ERROR_16, file_location);
                error_found = 1;
                continue;
            } 
          /* Save current file position */  
            if (fgetpos(fp_in, &pos)!=0) {
                print_internal_error(ERROR_11);
                fclose(fp_in);
                fclose(fp_out);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }
            
            /* Save macro content */
            content = save_macro_content(fp_in, &pos, &line_count,&error_code);
            if (content == NULL) {
                if (error_code==ERROR_1||error_code==ERROR_11) {
                    print_internal_error(error_code);
                    fclose(fp_in);
                    fclose(fp_out);
                    free_macro_table(table,table_size);
                    free(am_file);
                    return 0;
                }
                print_external_error(error_code, file_location);
                error_found = 1;
                continue;
            }

          /* Increase macro table size */
            temp = (macro_node *)realloc(table, (table_size + 1) * sizeof(macro_node));
            if (temp==NULL) {
                print_internal_error(ERROR_1);
                fclose(fp_in);
                fclose(fp_out);
                free(content);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }
            table = temp;
            table[table_size].name = NULL;
            table[table_size].content = NULL;

            /* Allocate memory for macro name */
            table[table_size].name = (char *)malloc(strlen(mcro_name) + 1);
            if (table[table_size].name==NULL) {
                print_internal_error(ERROR_1);
                fclose(fp_in);
                fclose(fp_out);
                free(content);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }
            /* Save macro name */
            strcpy(table[table_size].name, mcro_name);
            /* Save macro content */
            table[table_size].content = content;
            table_size++;
            continue;
        }

   
        found = 0;
        /* check if line is a mcro call */
        for (i = 0; i < table_size; i++) {
            if (is_macro_call(line,table[i].name)!=0) {
                /* expand macro into .am file */
                fprintf(fp_out, "%s", table[i].content);
                found = 1;
                break;
            }
        }

        /* Regular line */
        if (!found) {
            fprintf(fp_out, "%s", line);
        }

    }
    fclose(fp_in);
    fclose(fp_out);
    free(am_file);
    if (error_found) {
        free_macro_table(table,table_size);
        *macro_table=NULL;
        *macro_count=0;
        return 0;
    }
    *macro_table=table;
    *macro_count=table_size;
    return 1;
}
