#include "vector_config.h"
#include <stdlib.h>


// int
// VECTOR_TYPE VECTOR_COPY(VECTOR_TYPE value) {
//     return value;
// }

// void VECTOR_DELETE(VECTOR_TYPE value) {
//     (void)value;
// }


// char*
#include <string.h>
VECTOR_TYPE VECTOR_COPY(VECTOR_TYPE value) {
    if (!value) return NULL;
    char* new_str = (char*)malloc(strlen(value) + 1);
    if (new_str) strcpy(new_str, value);
    return new_str;
}

void VECTOR_DELETE(VECTOR_TYPE value) {
    free(value);
}