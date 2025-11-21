#ifndef VECTOR_H
#define VECTOR_H


#include <stddef.h>

#ifndef VECTOR_TYPE
#error "VECTOR_TYPE must be defined before including vector.h"
#endif

VECTOR_TYPE VECTOR_COPY(VECTOR_TYPE value);

void VECTOR_DELETE(VECTOR_TYPE value);

// #define VECTOR_TYPE char*
// char* vector_copy(char* value) { return strdup(value); }
// void vector_delete(char* value) { free(value); }



typedef struct {
    VECTOR_TYPE *data;
    size_t size;
    size_t capacity;
    VECTOR_TYPE (*CopyFunc)(VECTOR_TYPE);
    void (*DeleteFunc)(VECTOR_TYPE);
} Vector;

Vector create_vector(size_t initial_capacity, VECTOR_TYPE (*CopyFunc)(VECTOR_TYPE), void (*DeleteFunc)(VECTOR_TYPE));
void erase_vector(Vector *v);
int is_equal_vector(const Vector *v1, const Vector *v2);
void copy_vector(Vector *dest, const Vector *src);
Vector *copy_vector_new(const Vector *src);
void push_back_vector(Vector *v, VECTOR_TYPE value);
void delete_at_vector(Vector *v, size_t index);
VECTOR_TYPE get_at_vector(const Vector *v, size_t index);
void delete_vector(Vector *v);

#endif