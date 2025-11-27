#include "foo.h"
#include <limits.h>
// #include "SQ.h"
// #include "LinkedList.h"
// #include "Liver.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32  
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

UndoAction* create_undo_action(Action action, size_t index, const Liver *old_val, const Liver *new_val) {
    UndoAction *act = (UndoAction *)malloc(sizeof(UndoAction));
    if (!act) return NULL;
    act->action = action;
    act->index = index;
    act->old_value = old_val ? liver_copy(old_val) : NULL;
    act->new_value = new_val ? liver_copy(new_val) : NULL;
    return act;
}

void free_undo_action(UndoAction *act) {
    if (act) {
        liver_free(act->old_value);
        liver_free(act->new_value);
        free(act);
    }
}

Status undo_last_actions(LinkedList *undo_stack, LinkedList *list) {
    if (!undo_stack || !list || undo_stack->size == 0) return STATUS_OK;

    size_t n = undo_stack->size;
    size_t to_undo = (n + 1) / 2;  

    for (size_t i = 0; i < to_undo && undo_stack->size > 0; ++i) {
        UndoAction *act = pop_stack(undo_stack);  
        if (!act) break;

        switch (act->action) {
            case ACTION_INSERT:
                delete_at_list(list, act->index);  
                break;
            case ACTION_DELETE:
                if (act->old_value) {
                    insert_at_list(list, act->index, act->old_value);
                    act->old_value = NULL; 
                }
                break;
            case ACTION_UPDATE:
                if (act->old_value) {
                    Node *node = list->head;
                    for (size_t j = 0; j < act->index && node; j++) node = node->next;
                    if (node && node->data) {
                        liver_free(node->data);    
                        node->data = act->old_value;  
                        act->old_value = NULL;
                    }
                }
                break;
        }
        free_undo_action(act);
    }
    return STATUS_OK;
}

Status stroka_to_uint(const char *stroka, unsigned int *chislo) {
    if (!stroka || !chislo) return STATUS_INVALID_NUMBER;

    char *endptr = NULL;
    long long x = strtol(stroka, &endptr, 10);

    if (*endptr != '\0') return STATUS_INVALID_NUMBER;  
    if (stroka == endptr) return STATUS_INVALID_NUMBER; 
    if (x < 0 || x > UINT_MAX) return STATUS_TOO_BIG_INT; 

    *chislo = (unsigned int)x;
    return STATUS_OK;
}


int is_valid_date(const int day, const int year, const int month) {
    if (month < 1 || month > 12) return 0;
    
    int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    
    return (day >= 1 && day <= days_in_month[month - 1]);
}


int is_valid_name(const char *str) {
    if (!str || *str == '\0') return 0;
    char *p = (char*)str;
    while(*p) {
        if (!isalpha(*p)) return 0;
        *p++;
    } 
    return 1;
}

Status same_files(const char *file1, const char *file2) {
#ifdef _WIN32  
    BY_HANDLE_FILE_INFORMATION info1, info2;  
    HANDLE hFile1 = CreateFile(file1,   
                            GENERIC_READ,   
                            FILE_SHARE_READ,   
                            NULL,   
                            OPEN_EXISTING,   
                            FILE_ATTRIBUTE_NORMAL,   
                            NULL);  
    
    HANDLE hFile2 = CreateFile(file2,   
                            GENERIC_READ,   
                            FILE_SHARE_READ,   
                            NULL,   
                            OPEN_EXISTING,   
                            FILE_ATTRIBUTE_NORMAL,   
                            NULL);  
    
   if (hFile1 == INVALID_HANDLE_VALUE || hFile2 == INVALID_HANDLE_VALUE) {
        if (hFile1 != INVALID_HANDLE_VALUE) CloseHandle(hFile1);
        if (hFile2 != INVALID_HANDLE_VALUE) CloseHandle(hFile2);
        return STATUS_FILE_ERROR;
    }
    
    BOOL success1 = GetFileInformationByHandle(hFile1, &info1);  
    BOOL success2 = GetFileInformationByHandle(hFile2, &info2);  
    CloseHandle(hFile1);  
    CloseHandle(hFile2);  
    
    if (!success1 || !success2) {
        return STATUS_FILE_ERROR;
    }
    
    if (info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber &&  
        info1.nFileIndexHigh == info2.nFileIndexHigh &&  
        info1.nFileIndexLow == info2.nFileIndexLow) {
        return STATUS_SAME_FILES;
    }
    return STATUS_OK;
#else  
    struct stat st1, st2;
    if (stat(file1, &st1) != 0) return STATUS_OK;    
    if (stat(file2, &st2) != 0) return STATUS_OK;    
    
    if ((st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino)) {
        return STATUS_SAME_FILES;
    }
    return STATUS_OK;
#endif  
    return STATUS_FILE_ERROR;  
}

Status load_file(const char *filename, LinkedList *list) {
    if (!filename || !list) return STATUS_INVALID_INPUT;

    FILE *f = fopen(filename, "r");
    if (!f) return STATUS_FILE_ERROR;

    char line[512];
    // Считываем заголовок
    if (!fgets(line, sizeof(line), f)) { fclose(f); return STATUS_INVALID_INPUT; }

    while (fgets(line, sizeof(line), f)) {
        Liver l = {0};
        
        char *tokens[7];
        char *current = line;
        char *next_comma;
        int fields_count = 0;

        for (int i = 0; i < 6; ++i) {
            next_comma = strchr(current, ',');
            
            if (next_comma == NULL) { 
                fclose(f);
                return STATUS_INVALID_INPUT;
            }
            
            *next_comma = '\0'; 
            tokens[fields_count++] = current;
            current = next_comma + 1;
        }
        
        if (strchr(current, ',') != NULL) {
            fclose(f);
            return STATUS_INVALID_INPUT;
        }

        char *newline = strchr(current, '\n');
        if (newline) {
            *newline = '\0'; 
        }
        tokens[fields_count++] = current;

        if (fields_count != 7) { 
            fclose(f);
            return STATUS_INVALID_INPUT;
        }


        unsigned int id;
        char gender;
        double avg_income;
        int d = 0, m = 0, y = 0;
        char patronymic[64];
        
        if (sscanf(tokens[0], "%u", &id) != 1) { fclose(f); return STATUS_INVALID_INPUT; }
        l.id = id;

        if (strlen(tokens[3]) > 0) {
            strcpy(patronymic, tokens[3]);
        } else {
            patronymic[0] = '\0';
        }

        if (sscanf(tokens[4], "%d.%d.%d", &d, &m, &y) != 3) { fclose(f); return STATUS_INVALID_INPUT; }
        l.birth_date.day = d;
        l.birth_date.month = m;
        l.birth_date.year = y;
        
        if (sscanf(tokens[5], "%c", &gender) != 1) { fclose(f); return STATUS_INVALID_INPUT; }
        l.gender = gender;

        if (sscanf(tokens[6], "%lf", &avg_income) != 1) { fclose(f); return STATUS_INVALID_INPUT; }
        l.avg_income = avg_income;


        if (!is_valid_name(tokens[1]) || ! is_valid_name(tokens[2]) || (strlen(patronymic) > 0 && !is_valid_name(patronymic)) ||
            (gender != 'M' && gender != 'W') || avg_income < 0.0 || !is_valid_date(d, y, m)) { 
                fclose(f);
                return STATUS_INVALID_INPUT;
            }
        
        strcpy(l.name, tokens[2]);
        strcpy(l.surname, tokens[1]);
        strcpy(l.patronymic, patronymic);


        Node *cur = list->head;
        size_t index = 0;

        while (cur && liver_cmp_by_age(&l, (const Liver*)cur->data) > 0) {
            cur = cur->next;
            index++;
        }
        insert_at_list(list, index, &l);
    }

    fclose(f);
    return STATUS_OK;
}

Status save_file(const char *filename, LinkedList *list) {
    if (!filename || !list) return STATUS_INVALID_INPUT;

    FILE *f = fopen(filename, "w");
    if (!f) return STATUS_FILE_ERROR;

    fprintf(f, "id,surname,name,patronymic,day.month.year,sex,income\n");
    Node *cur = list->head;
    while (cur) {
        Liver *l = (Liver*)cur->data;
                fprintf(f, "%u,%s,%s,%s,%02d.%02d.%04d,%c,%.2f\n",
                l->id, l->surname, l->name, l->patronymic,
                l->birth_date.day,
                l->birth_date.month,
                l->birth_date.year,
                l->gender,
                l->avg_income);
        cur = cur->next;
    }
    fclose(f);
    return STATUS_OK;
}


void* undo_action_copy_func(const void *src) {
    if (!src) return NULL;
    const UndoAction *act_src = (const UndoAction *)src;

    return (void*)create_undo_action(
        act_src->action, 
        act_src->index, 
        act_src->old_value, 
        act_src->new_value
    );
}

void undo_action_free_func(void *data) {
    free_undo_action((UndoAction *)data);
}