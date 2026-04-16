//
// Created by ethan on 09/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_struct.h"
#include "pre_proc.h"
#define MAX_LINE_LENGTH 81


static void free_macro_table(macro_node *table, int table_size) {
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

int is_macro(char *line_num,char **mcro_name) {
    char *token;
    char *extra;
    token= strtok(line_num," \t\n");
    if (token == NULL) return 0;
    if (strcmp(token,"mcro")!=0) {
        return 0;
    }
    token= strtok(NULL," \t\n");
    if (token == NULL) return -1;
    *mcro_name=token;
    extra = strtok(NULL," \t\n");
    if (extra != NULL) return -1;
    return 1;
}

char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count) {
    int mcro_length=0;
    char *mcro;
    char str[MAX_LINE_LENGTH];
    if (fsetpos(fp,pos)!=0) {
        return NULL;
    }
    str[0]='\0';
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL &&strcmp(str,"endmcro\n")!=0) {
        mcro_length+=(int)strlen(str);
        (*line_count)++;
    }
    mcro = (char *)malloc(mcro_length+1);
    if (mcro==NULL)return NULL;
    if (fsetpos(fp,pos)!=0) {
        free(mcro);
        return NULL;
    }
    mcro[0]= '\0';
    while (fgets(str,MAX_LINE_LENGTH,fp)!=NULL &&strcmp(str,"endmcro\n")!=0) {
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

int run_preproc(char *file_name) {
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
    /* פתיחת קבצים */
    fp_in = fopen(file_name, "r");
    if (fp_in == NULL) return 0;

    am_file = add_new_file(file_name, ".am");
    if (am_file==NULL) {
        fclose(fp_in);
        return 0;
    }
    fp_out = fopen(am_file, "w");
    if (fp_out == NULL) {
        fclose(fp_in);
        free(am_file);
        return 0;
    }

    while (fgets(line,MAX_LINE_LENGTH , fp_in)) {
        line_count++;
        strcpy(line_copy, line);
        /* בדוק אם זו הגדרת מאקרו */
        macro_status=is_macro(line_copy,&mcro_name);
        if (macro_status==-1) {
            fclose(fp_in);
            fclose(fp_out);
            free_macro_table(table,table_size);
            free(am_file);
            return 0;
        }
        if (macro_status==1) {
            /* שמור מיקום תחילת תוכן המאקרו */
            if (fgetpos(fp_in, &pos)!=0) {
                fclose(fp_in);
                fclose(fp_out);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }
            content = save_macro_content(fp_in, &pos, &line_count);
            if (content == NULL) {
                fclose(fp_in);
                fclose(fp_out);
                free_macro_table(table,table_size);
                free(am_file);
                return 0;
            }

            /* הוסף לטבלה */
            temp = (macro_node *)realloc(table, (table_size + 1) * sizeof(macro_node));
            if (temp==NULL) {
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

    /* ניקוי */
    fclose(fp_in);
    fclose(fp_out);
    free_macro_table(table,table_size);
    free(am_file);
    return 1;
}
