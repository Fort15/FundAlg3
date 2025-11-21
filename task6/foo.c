#include "foo.h"


void Create(stack *s) {
    s->top = 0;
    s->size = 0;
}

bool Empty(stack *s) {
    return s->top == 0;
}

int Size(stack *s) {
    return s->size;
}

bool Push(stack *s, char t) {
    struct Item* i = malloc(sizeof(struct Item));
    if (!i) return false;
    i->data = t;
    i->prev = s->top;
    s->top = i;
    s->size++;
    return true;
}

bool Pop(stack *s) {
    if (Empty(s)) return false;

    struct Item* i = s->top;
    s->top = s->top->prev;
    s->size--;
    free(i);
    return true;
}

char Top(stack *s) {
    if (s->top) return s->top->data;
}

void Destroy(stack *s) {
    while (s->top) {
        struct Item* i = s->top;
        s->top = s->top->prev;
        free(i);
    }
    s->top = 0;
    s->size = 0;
}

int check_brackets(const char *str) {
    char *p = (char *)str;
    stack s;
    Create(&s);
    while (*p) {
        if (*p == '(' || *p == '{' || *p == '[' || *p == '<') Push(&s, *p);
        else if (*p == ')' || *p == '}' || *p == ']' || *p == '>') {
            if (Empty(&s)) {Destroy(&s); return 1;}
            char c = Top(&s);
            if (c == '(' && *p != ')' || c == '[' && *p != ']' || c == '{' && *p != '}' ||
                c == '<' && *p != '>') {Destroy(&s); return 1;}
            Pop(&s);
        }
        p++;
    }

    if (!Empty(&s)) {Destroy(&s); return 1;}
    Destroy(&s);
    return 0;
}
