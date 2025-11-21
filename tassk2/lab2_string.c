#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector_config.h"
#include "vector.h"

int main() {
    Vector v = create_vector(2, VECTOR_COPY, VECTOR_DELETE);
    if (!v.data) {
        printf("Ошибка создания вектора\n");
        return 1;
    }

    push_back_vector(&v, "Alex");
    push_back_vector(&v, "John");
    push_back_vector(&v, "Obama");

    printf("v[0]=%s, v[1]=%s, v[2]=%s\n",
           get_at_vector(&v, 0), get_at_vector(&v, 1), get_at_vector(&v, 2));

    // Копирование
    Vector v2 = create_vector(1, VECTOR_COPY, VECTOR_DELETE);
    copy_vector(&v2, &v);
    printf("v2 size: %zu, v2[2]=%s\n", v2.size, get_at_vector(&v2, 2));
    printf("v2==v? %s\n", is_equal_vector(&v, &v2) ? "YES" : "NO");

    // Новый вектор
    Vector *v3 = copy_vector_new(&v);
    if (v3) {
        printf("v3 size: %zu, v3==v? %s\n", v3->size,
               is_equal_vector(&v, v3) ? "YES" : "NO");
        delete_vector(v3);
    }

    delete_at_vector(&v, 1);
    printf("delete_at_vector(&v, 1): v[0]=%s, v[1]=%s, size=%zu\n",
           get_at_vector(&v, 0), get_at_vector(&v, 1), v.size);

    erase_vector(&v);
    erase_vector(&v2);

    return 0;
}