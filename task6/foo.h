#ifndef FOO_H
#define FOO_H

#include <stdlib.h>
#include <stdbool.h>


struct Item {
    char data;
    struct Item* prev;
};

typedef struct {
    int size;
    struct Item* top;
} stack;

void Create(stack *s);
bool Empty(stack *s);
int Size(stack *s);
bool Push(stack *s, char t);
bool Pop(stack *s);
char Top(stack *s);
void Destroy(stack *s);
int check_brackets(const char *str);

#endif