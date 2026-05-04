//
// Created by ethan on 09/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_struct.h"
#include "pre_proc.h"
#include "errors.h"
#include "first_pass.h"
#define MAX_LINE_LENGTH 81

void free_macro_table(macro_node *table, int table_size) {
    int i;
    for (i = 0; i < table_size; i++) {
        free(table[i].name);
        free(table[i].content);
    }
    free(table);
}

char *add_new_file(char *file_name, char *ending) {
    char *dot;
    char *new_file_name;

    new_file_name = (char *)malloc(strlen(file_name) + strlen(ending) + 1);
    if (new_file_name == NULL) {
        return NULL;
    }

    strcpy(new_file_name, file_name);

    dot = strchr(new_file_name, '.');
    if (dot != NULL) {
        *dot = '\0';
    }

    strcat(new_file_name, ending);
    return new_file_name;
}

int is_macro(char *line_num,char **mcro_name,ERROR_CODES *error_code) {
    char *token;
    char *extra;
    token= strtok(line_num," \t\n");
    if (token == NULL) return 0;
    if (strcmp(token,"mcro")!=0) {
        return 0;
    }
    token= strtok(NULL," \t\n");
    if (token == NULL) {
        *error_code=ERROR_9;
        return -1;
    }
    *mcro_name=token;
    extra = strtok(NULL," \t\n");
    if (extra != NULL) {
        *error_code=ERROR_10;
        return -1;
    }
    return 1;
}

char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count,ERROR_CODES *error_code) {
    char *p;
    int found_end=0;
    int mcro_length=0;
    char *mcro;
    char str[MAX_LINE_LENGTH];
    char str_copy[MAX_LINE_LENGTH];
    if (fsetpos(fp,pos)!=0) {
        *error_code=ERROR_11;
        return NULL;
    }
    str[0]='\0';
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL) {
        (*line_count)++;
        strcpy(str_copy,str);
        trim_spaces(str_copy);
        if (strncmp(str_copy,"mcroend",7)==0) {
            p=str_copy+7;
            while (*p==' '||*p=='\t') {
                p++;
            }
            if (*p!='\n'&&*p!='\0') {
                *error_code=ERROR_12;
                return NULL;
            }
            found_end=1;
            break;
        }
        mcro_length+=(int)strlen(str);
    }
    if (!found_end) {
        *error_code=ERROR_15;
        return NULL;
    }
    mcro = (char *)malloc(mcro_length+1);
    if (mcro==NULL) {
        *error_code=ERROR_1;
        return NULL;
    }
    if (fsetpos(fp,pos)!=0) {
        *error_code=ERROR_11;
        free(mcro);
        return NULL;
    }
    mcro[0]= '\0';
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL &&strncmp(str,"mcroend",7)!=0) {
        strcat(mcro,str);
    }
    return mcro;
}
int is_macro_call(char *line, char *macro_name) {
    char copy[MAX_LINE_LENGTH];
    char *token;
    char *extra;
    strcpy(copy, line);
    token = strtok(copy, " \t\n");
    if (token == NULL) {
        return 0;
    }
    if (strcmp(token, macro_name) != 0) {
        return 0;
    }
    extra = strtok(NULL, " \t\n");
    if (extra != NULL) {
        return 0;
    }
    return 1;
}
int find_macro(macro_node *table, int table_size, char *name) {
    int i;
    if(table ==NULL || name == NULL) {
        return -1;
    }
    for (i =0; i<table_size;i++) {
        if(strcmp(table[i].name, name) == 0) {
            return 1;
        }
    }
    return -1;
}
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
    while (fgets(line,MAX_LINE_LENGTH , fp_in)) {
        line_count++;
        file_location.file_name = file_name;
        file_location.line_num = line_count;
        strcpy(line_copy, line);
        macro_status=is_macro(line_copy,&mcro_name,&error_code);
        if (macro_status==-1) {
            print_external_error(error_code,file_location);
            error_found=1;
            continue;
        }
        if (macro_status==1) {
            /* שמור מיקום תחילת תוכן המאקרו */
            if (fgetpos(fp_in, &pos)!=0) {
                print_internal_error(ERROR_11);
                fclose(fp_in);
                fclose(fp_out);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }
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

            /* הוסף לטבלה */
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
            strcpy(table[table_size].name, mcro_name);
            table[table_size].content = content;
            table_size++;
            continue;
        }

        /* בדוק אם השורה היא קריאה למאקרו */
        found = 0;
        for (i = 0; i < table_size; i++) {
            if (is_macro_call(line,table[i].name)!=0) {
                fprintf(fp_out, "%s", table[i].content);
                found = 1;
                break;
            }
        }

        /* שורה רגילה */
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
