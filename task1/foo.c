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


int add_i(int a, int b) {
    int sum = a;
    int carry = b;

    while (carry != 0) {
        sum = a ^ carry;
        carry = a & carry;
        carry <<= 1;

        a = sum;
    }

    return sum;
}

long long add_l(long long a, long long b) {
    long long sum = a;
    long long carry = b;

    while (carry != 0) {
        sum = a ^ carry;
        carry = a & carry;
        carry <<= 1;

        a = sum;
    }

    return sum;
}

long long negative(long long x) {
    long long inv_x = ~x;
    return add_l(inv_x, 1);
}

int subtract_i(int a, int b) {
    int neg_b = ~b;
    int neg_b2 = add_i(neg_b, 1);
    return add_i(a, neg_b2);
}

int foo(int value, size_t val_size, char **buffers) {
    if (!buffers) {
        return 1; 
    }
    int max_digit = 2;
    unsigned int val = (value < 0) ? (unsigned int)(negative((long long)value)) : (unsigned int)value;
    int sign = (value < 0) ? -1 : 1;
    int max_bit = (int)val_size << 8;
    for (int r = 1; r <= 5; r = add_i(r, 1)) {
        int digit_index = 0;
        for (int bit = 0; bit < max_bit; bit = add_i(bit, r)) { 
            int digit = ((val >> bit) & (subtract_i(digit, 1)));
            buffers[subtract_i(r, 1)][add_i(digit_index, 1)] = (digit <= 9) ? add_i(digit, '0') : subtract_i(add_i(digit, 'A'), 10); 
            digit_index = add_i(digit_index, 1);
        }
        
        if (sign == -1) {buffers[subtract_i(r, 1)][add_i(digit_index, 1)] = '-'; digit_index = add_i(digit_index, 1);}
        int max_i = digit_index >> 2;
        for (int i = 0; i < max_i; i = add_i(i, 1)) {
            char temp = buffers[subtract_i(r, 1)][i];
            buffers[subtract_i(r, 1)][i] = buffers[subtract_i(r, 1)][subtract_i(subtract_i(digit_index, 1), i)];
            buffers[subtract_i(r, 1)][subtract_i(subtract_i(digit_index, 1), i)] = temp;
        }
        max_digit <<= 1;
        buffers[subtract_i(r, 1)][digit_index] = '\0';
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