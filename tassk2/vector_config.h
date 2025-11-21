#ifndef VECTOR_CONFIG_H
#define VECTOR_CONFIG_H

// #define VECTOR_TYPE int  // int
#define VECTOR_TYPE char*  // char *
#define VECTOR_COMPARE_STRINGS

VECTOR_TYPE VECTOR_COPY(VECTOR_TYPE value);
void VECTOR_DELETE(VECTOR_TYPE value);


#endif