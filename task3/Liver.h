#ifndef LIVER_H
#define LIVER_H

typedef struct {
    int day;
    int month; 
    int year;  
} Date;

typedef struct {
    unsigned int id;
    char surname[64];
    char name[64];
    char patronymic[64];  // отчество
    Date birth_date;
    char gender;
    double avg_income;
} Liver;

int liver_cmp_by_age(const Liver *a, const Liver *b);
int liver_equal(const Liver *a, const Liver *b);
Liver *liver_copy(const Liver *src);
void liver_free(Liver *l);

void* liver_copy_func(const void *src);
void liver_free_func(void *data);

#endif