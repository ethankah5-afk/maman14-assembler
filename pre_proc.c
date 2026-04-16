//
// Created by ethan on 09/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pre_proc.h"


int is_macro(char *line_num,char **mcro_name) {
    char *token;
    char *extra;
    token= strtok(line_num," /t/n");
    if (token == NULL) return 0;
    if (strcmp(token,"mcro")!=0) {
        return 0;
    }
    token= strtok(NULL," /t/n");
    if (token == NULL) return -1;
    *mcro_name=token;
    extra = strtok(NULL," /t/n");
    if (extra == NULL) return -1;
    return 1;
}

char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count) {
    int mcro_length=0;
    char *mcro;
    char str[81];
    if (fsetpos(fp,pos)!=0) {
        return NULL;
    }
    str[0]='\0';
    while (fgets(str,81,fp)!=NULL &&strcmp(str,"endmcro\n")!=0) {
        mcro_length+=(int)strlen(str);
        (*line_count)++;
    }
    mcro = (char *)malloc(mcro_length+1);
    if (mcro==NULL)return NULL;
    if (fsetpos(fp,pos)!=0) {
        free(mcro);
        return NULL;
    }
    mcro[0]= '/0';
    while (fgets(str,81,fp)!=NULL &&strcmp(str,"endmcro\n")!=0) {
        strcat(mcro,str);
    }
    return mcro;
}

int is_macro_call(char *line, char *macro_name) {
    char copy[81];
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
    char line[80];
    char line_copy[80];
    char *mcro_name, *am_file, *content;
    fpos_t pos;
    macro_node *table = NULL;
    macro_node *temp;
    int table_size = 0;
    int line_count = 0;
    int is_valid_nacro_name;
    int found;
    int i;
    /* פתיחת קבצים */
    fp_in = fopen(file_name, "r");
    if (fp_in == NULL) return 0;

    am_file = add_new_file(file_name, ".am");
    fp_out = fopen(am_file, "w");
    if (fp_out == NULL) {
        fclose(fp_in);
        return 0;
    }

    while (fgets(line, 80, fp_in)) {
        line_count++;
        strcpy(line_copy, line);
        /* בדוק אם זו הגדרת מאקרו */
        is_valid_nacro_name=is_macro(line_copy,&mcro_name);
        if (is_valid_nacro_name==-1) {
            fclose(fp_in);
            fclose(fp_out);
            free(am_file);
        }
        if (is_valid_nacro_name==1) {
            /* שמור מיקום תחילת תוכן המאקרו */
            fgetpos(fp_in, &pos);
            /* שמור את תוכן המאקרו */
            content = save_macro_content(fp_in, &pos, &line_count);
            if (content == NULL) {
                fclose(fp_in);
                fclose(fp_out);
                free(am_file);
                return 0;
            }

            /* הוסף לטבלה */
            temp = realloc(table, (table_size + 1) * sizeof(macro_node));
            if (temp==NULL) {
                fclose(fp_in);
                fclose(fp_out);
                free(am_file);
                return 0;
            }
            table = temp;

            table[table_size].name = (char *)malloc(strlen(mcro_name) + 1);
            if (table[table_size].name==NULL) {
                fclose(fp_in);
                fclose(fp_out);
                free(am_file);
                return 0;
            }
            strcpy(table[table_size].name, mcro_name);
            table[table_size].content = content;
            table_size++;

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
    for (i = 0; i < table_size; i++) {
        free(table[i].name);
        free(table[i].content);
    }
    free(table);
    free(am_file);

    return 1;
}
