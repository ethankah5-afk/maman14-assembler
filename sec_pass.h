//
// Created by ethan on 23/04/2026.
//

#ifndef ASSEMBLERPROJECT_SEC_PASS_H
#define ASSEMBLERPROJECT_SEC_PASS_H

Label *find_label_by_name(LableTable *labels, const char *name); 
int is_extern_name(NameRefTable *externs, const char *name); 
int resolve_one_word(CodeWord *word, LableTable *labels, NameRefTable *externs); 
#endif //ASSEMBLERPROJECT_SEC_PASS_H
