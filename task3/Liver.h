#ifndef LIVER_H
#define LIVER_H

#include <time.h>

typedef struct {
    unsigned int id;
    char surname[64];   
    char name[64];     
    char patronymic[64]; 
    struct tm birth_date; 
    char gender;      
    double avg_income; 
} Liver;

#endif