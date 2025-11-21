#include "foo.h"
#include <limits.h>
#include <stdlib.h>

Status stroka_to_int(const char *stroka, int *chislo) {
    if (!stroka || !chislo) return STATUS_INVALID_INPUT;

    char *endptr = NULL;
    long x = strtol(stroka, &endptr, 10);

    while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n') {
        endptr++;
    }

    if (*endptr != '\0') return STATUS_INVALID_INPUT;
    if (stroka == endptr) return STATUS_INVALID_INPUT;
    if (x > INT_MAX || x < INT_MIN) return STATUS_TOO_BIG_INT;

    *chislo = (int)x;
    return STATUS_OK;
}


int foo(int value, size_t val_size, char **buffers) {
    if (!buffers) {
        return 1; 
    }
    int max_digit = 2;
    unsigned int val = (value < 0) ? (unsigned int)(-(long long)value) : (unsigned int)value;
    int sign = (value < 0) ? -1 : 1;
    for (int r = 1; r <= 5; ++r) {
        int digit_index = 0;
        for (size_t bit = 0; bit < val_size * 8; bit += r) {
            int digit = ((val >> bit) & (max_digit - 1));
            buffers[r - 1][digit_index++] = (digit <= 9) ? digit + '0' : digit + 'A' - 10;
        }
        
        if (sign == -1) buffers[r - 1][digit_index++] = '-';
        for (int i = 0; i < digit_index / 2; i++) {
            char temp = buffers[r - 1][i];
            buffers[r - 1][i] = buffers[r - 1][digit_index - 1 - i];
            buffers[r - 1][digit_index - 1 - i] = temp;
        }
        max_digit <<= 1;
        buffers[r - 1][digit_index] = '\0';
    }
    return 0;
}

char* normalize(char* str) {
    char* p = str;
    int sign = (*p == '-') ? -1 : 1;
    if (*p == '-' || *p == '+') p++;
    while(*p == '0' && *(p + 1) != '\0') {
        p++;
    }
    if (sign == -1) *(--p) = '-';
    return p;
}