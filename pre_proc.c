//
// Created by ethan on 09/03/2026.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pre_proc.h"


int is_macro(char *line_num,char **mcro_name) {
    if (strcmp(strtok(line_num," "),"mcro")==0) {
        *mcro_name=strtok(NULL," ");
        return 1;
    }
    return 0;
}

char* save_macro_content(FILE *fp, fpos_t* pos, int *line_count) {
    int mcro_length=0;
    char *mcro;
    char str[80];
    if (fsetpos(fp,pos)!=0) {
        return NULL;
    }
    str[0]='\0';
    while (fgets(str,80,fp)&&(strcmp(str,"endmcro\n"))!=0) {
        if (strstr(str,"endmcro")!=NULL&&strlen(str)!=strlen("endmcro")) {
            return NULL;
        }
        (*line_count)++;
        if (strcmp(str,"endmcro\n")!=0) {
            mcro_length+=strlen(str);
        }
    }
    /*mcro = copy_text(fp, pos, mcro_length);*/
    return mcro;
}

int run_preproc(char *file_name) {
    FILE *fp_in, *fp_out;
    char line[80];
    char line_copy[80];
    char *mcro_name, *am_file, *content;
    fpos_t pos;
    struct macro_table *table = NULL;
    int table_size = 0;
    int line_count = 0;
    int i, found;

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
        if (is_macro(line_copy, &mcro_name)) {

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
            table = realloc(table, (table_size + 1) * sizeof(macro_node));
            table[table_size].name = malloc(strlen(mcro_name) + 1);
            strcpy(table[table_size].name, mcro_name);
            table[table_size].content = content;
            table_size++;

            /* קפוץ לאחרי endmcro */
            fsetpos(fp_in, &pos);
            continue;
        }

        /* בדוק אם השורה היא קריאה למאקרו */
        found = 0;
        strcpy(line_copy, line);
        for (i = 0; i < table_size; i++) {
            if (strstr(line_copy, table[i].name) != NULL) {
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
