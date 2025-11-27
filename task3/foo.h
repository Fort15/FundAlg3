#ifndef FOO_H
#define FOO_H

#include "LinkedList.h"

typedef enum {
    STATUS_OK,
    STATUS_FILE_ERROR,
    STATUS_INVALID_INPUT,
    STATUS_MEMORY_ERROR,
    STATUS_SAME_FILES,
    STATUS_INVALID_NUMBER,
    STATUS_TOO_BIG_INT
} Status;


UndoAction* create_undo_action(Action action, size_t index, const Liver *old_val, const Liver *new_val);
void free_undo_action(UndoAction *act);


Status undo_last_actions(LinkedList *undo_stack, LinkedList *list);


Status stroka_to_uint(const char *stroka, unsigned int *chislo);

int is_valid_name(const char *str);
int is_valid_date(const int day, const int year, const int month);


Status same_files(const char *file1, const char *file2);
Status load_file(const char *filename, LinkedList *list);
Status save_file(const char *filename, LinkedList *list);

void* undo_action_copy_func(const void *src);
void undo_action_free_func(void *data);

#endif