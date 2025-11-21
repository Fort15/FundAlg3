#ifndef FOO_H
#define FOO_H

#include <stddef.h>

#define BUFSIZE 100

typedef enum {
    STATUS_OK,     
    STATUS_INVALID_INPUT,
    STATUS_TOO_BIG_INT,
} Status;


Status stroka_to_int(const char *stroka, int *chislo);
int foo(int value, size_t val_size, char **bufers);
char* normalize(char* str);


#endif