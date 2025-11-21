#include "vector.h"
#include <stdlib.h>

Vector create_vector(size_t initial_capacity, VECTOR_TYPE (*CopyFunc)(VECTOR_TYPE), void (*DeleteFunc)(VECTOR_TYPE)) {
    Vector v = {0};
    if (!CopyFunc || !DeleteFunc) return v;

    v.CopyFunc = CopyFunc;
    v.DeleteFunc = DeleteFunc;
    v.capacity = initial_capacity > 0 ? initial_capacity : 1;
    v.data = (VECTOR_TYPE *)malloc(v.capacity * sizeof(VECTOR_TYPE));

    if (!v.data) {
        v.capacity = 0;
        return v;
    }
    v.size = 0;
    return v;
}

void erase_vector(Vector *v) {
    if (!v || !v->data) return;
    for (size_t i = 0; i < v->size; ++i) {
        v->DeleteFunc(v->data[i]);
    }
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}

int is_equal_vector(const Vector *v1, const Vector *v2) {
    if (!v1 || !v2) return 0;
    if (v1->size != v2->size) return 0;
    for (size_t i = 0; i < v1->size; ++i) {
        VECTOR_TYPE a = v1->data[i];
        VECTOR_TYPE b = v2->data[i];
        if (!(a == b)) return 0;
    }
    return 1;
}

void copy_vector(Vector *dest, const Vector *src) {
    if (!dest || !src) return;
    if (dest == src) return;

    erase_vector(dest);
    dest->capacity = src->capacity;
    dest->size = src->size;
    dest->CopyFunc = src->CopyFunc;
    dest->DeleteFunc = src->DeleteFunc;

    dest->data = (VECTOR_TYPE *)malloc(dest->capacity * sizeof(VECTOR_TYPE));
    if (!dest->data) {
        dest->capacity = 0;
        dest->size = 0;
        return;
    }

    for (size_t i = 0; i < src->size; ++i) {
        dest->data[i] = src->CopyFunc(src->data[i]);
    }
}

Vector *copy_vector_new(const Vector *src) {
    if (!src) return NULL;

    Vector *v = (Vector *)malloc(sizeof(Vector));
    if (!v) return NULL;

    *v = create_vector(src->capacity, src->CopyFunc, src->DeleteFunc);
    copy_vector(v, src);

    if (!v->data) {
        free(v);
        return NULL;
    }
    return v;
}

void push_back_vector(Vector *v, VECTOR_TYPE value) {
    if (!v || !v->CopyFunc || !v->DeleteFunc) return;
    if (v->size >= v->capacity) {
        size_t new_cap = v->capacity ? v->capacity * 2 : 1;
        VECTOR_TYPE *new_data = (VECTOR_TYPE *)realloc(v->data, new_cap * sizeof(VECTOR_TYPE));
        if (!new_data) return; 
        v->data = new_data;
        v->capacity = new_cap;
    }
    v->data[v->size] = v->CopyFunc(value);
    v->size++;
}

void delete_at_vector(Vector *v, size_t index) {
    if (!v || index >= v->size) return;

    v->DeleteFunc(v->data[index]);

    for (size_t i = index; i < v->size - 1; ++i) {
        v->data[i] = v->data[i + 1];
    }
    v->size--;
}

VECTOR_TYPE get_at_vector(const Vector *v, size_t index) {
    if (!v || index >= v->size) {
        VECTOR_TYPE zero = {0};
        return zero;
    }
    return v->data[index];
}

void delete_vector(Vector *v) {
    if (!v) return;
    erase_vector(v);
}