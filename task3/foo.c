#include "foo.h"
// #include "SQ.h"
// #include "LinkedList.h"
// #include "Liver.h"
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


int is_valid_date(const int day, const int year, const int month) {
    if (month < 1 || month > 12) return 0;
    
    int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    
    return (day >= 1 && day <= days_in_month[month - 1]);
}


int is_valid_name(const char *str) {
    if (!str || *str == '\0') return 0;
    char *p = str;
    while(*p) {
        if (!isalpha(*p)) return;
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
    if (!fgets(line, sizeof(line), f)) { fclose(f); return STATUS_INVALID_INPUT; }

    while (fgets(line, sizeof(line), f)) {
        Liver l = {0};
        unsigned int id;
        char surname[64];
        char name[64];
        char patronymic[64];
        int d = 0, m = 0, y = 0;
        char gender;
        double avg_income;
        if (sscanf(line, "%u,%63[^,],%63[^,],%63[^,],%d.%d.%d,%c,%lf", 
            &id, &surname, &name, &patronymic, &d, &m, &y, &gender, &avg_income) != 9) {
                fclose(f);
                return STATUS_INVALID_INPUT;
        }

        if (!is_valid_name(surname) || ! is_valid_name(name) || (strlen(patronymic) > 0 && !is_valid_name(patronymic)) ||
            (gender != 'M' && gender != 'W') || avg_income < 0.0 || !is_valid_date(d, y, m)) { 
                fclose(f);
                return STATUS_INVALID_INPUT;
            }
        
        l.id = id;
        strcpy(l.name, name);
        strcpy(l.surname, surname);
        strcpy(l.patronymic, patronymic);
        l.birth_date.day = d;
        l.birth_date.month = m;
        l.birth_date.year = y;
        l.gender = gender;
        l.avg_income = avg_income;

        Node *cur = list->head;
        size_t index = 0;
        while (cur && liver_cmp_by_age(&l, cur->data) > 0) {
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
        Liver *l = cur->data;
                fprintf(f, "%u,%s,%s,%s,%02d.%02d.%04d,%c,%.2f\n",
                l->id, l->surname, l->name, l->patronymic,
                l->birth_date.day,
                l->birth_date.month,
                l->birth_date->year,
                l->gender,
                l->avg_income);
        cur = cur->next;
    }
    fclose(f);
    return STATUS_OK;
}