#include "Liver.h"
#include <stdlib.h>

int liver_cmp_by_age(const Liver *a, const Liver *b) {
    if (!a || !b) return 0;
    if (a->birth_date.year != b->birth_date.year)
        return b->birth_date.year - a->birth_date.year;
    if (a->birth_date.month != b->birth_date.month)
        return b->birth_date.month - a->birth_date.month;
    return b->birth_date.day - a->birth_date.day;
}

int liver_equal(const Liver *a, const Liver *b) {
    if (!a || !b) return 0;
    return a->id == b->id &&
           strcmp(a->surname, b->surname) == 0 &&
           strcmp(a->name, b->name) == 0 &&
           strcmp(a->patronymic, b->patronymic) == 0 &&
           a->birth_date.day == b->birth_date.day &&
           a->birth_date.month == b->birth_date.month &&
           a->birth_date.year == b->birth_date.year &&
           a->gender == b->gender &&
           (fabs(a->avg_income - b->avg_income) < 1e-9); 
}

Liver *liver_copy(const Liver *src) {
    if (!src) return NULL;
    Liver *dst = (Liver *)malloc(sizeof(Liver));
    if (!dst) return NULL;
    *dst = *src;
    return dst;
}

void liver_free(Liver *l) {
    free(l);
}