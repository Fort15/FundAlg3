#include <stdio.h>
#include <stddef.h>
#include "foo.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Неверное число аргументов\n");
        return 0;
    }

    int number;
    Status status = stroka_to_int(argv[1], &number);
    if (status != STATUS_OK) {
        printf("Некорректное число\n");
        return 0;
    }
    
    char number2[BUFSIZE], number4[BUFSIZE], number8[BUFSIZE], number16[BUFSIZE], number32[BUFSIZE];
    char* buffers[5] = {number2, number4, number8, number16, number32};
    if (foo(number, sizeof(number), buffers)) {printf("Ошибка преобразования\n"); return 0;}
    int base = 2;
    for (size_t i = 0; i <= 4; ++i) {
        printf("В %d: %s\n", base, normalize(buffers[i]));
        base <<= 1;
    }
    return 0;
}