#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VECTOR_TYPE int

#include "vector.h"
#include "vector.c"

VECTOR_TYPE VECTOR_COPY(VECTOR_TYPE value) { return value; }
void VECTOR_DELETE(VECTOR_TYPE value) { (void)value; }


int main() {
    Vector v = create_vector(2, VECTOR_COPY, VECTOR_DELETE);
    if (!v.data) {
        printf("Ошибка создания вектора\n");
        return 1;
    }

    push_back_vector(&v, 10);
    push_back_vector(&v, 20);
    push_back_vector(&v, 30);

    printf("v[0]=%d, v[1]=%d, v[2]=%d\n",
           get_at_vector(&v, 0), get_at_vector(&v, 1), get_at_vector(&v, 2));

    // Копирование
    Vector v2 = create_vector(1, VECTOR_COPY, VECTOR_DELETE);
    copy_vector(&v2, &v);
    printf("v2 size: %zu, v2[2]=%d\n", v2.size, get_at_vector(&v2, 2));

    // Новый вектор
    Vector *v3 = copy_vector_new(&v);
    if (v3) {
        printf("v3 size: %zu, equal to v? %s\n", v3->size,
               is_equal_vector(&v, v3) ? "YES" : "NO");
        delete_vector(v3);
        free(v3);
    }

    delete_at_vector(&v, 1);
    printf("After delete v[1]: v[0]=%d, v[1]=%d, size=%zu\n",
           get_at_vector(&v, 0), get_at_vector(&v, 1), v.size);

    erase_vector(&v);
    erase_vector(&v2);

    printf("✅ Все тесты пройдены.\n");
    return 0;
}