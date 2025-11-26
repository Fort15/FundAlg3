#ifndef FOO_H
#define FOO_H

#include "LinkedList.h"

typedef enum {
    STATUS_OK,
    STATUS_FILE_ERROR,
    STATUS_INVALID_INPUT,
    STATUS_MEMORY_ERROR,
    STATUS_SAME_FILES
} Status;

int is_valid_name(const char *str);
int is_valid_date(const Date* date);

Status same_files(const char *file1, const char *file2);
Status load_file(const char *filename, LinkedList *list);
Status save_file(const char *filename, LinkedList *list);

#endif