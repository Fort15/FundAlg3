#include <stdio.h>
#include "foo.h"
#include <string.h>


int main() {
    char line[1024];
    if (!fgets(line, sizeof(line), stdin)) {
        printf("Ошибка чтения строки\n");
        return 0;
    }

    if (!strchr(line, '\n')) {
        printf("Слишком длинная строка\n");
        return 0;
    }
    line[strcspn(line, "\n")] = '\0';
    stack s;
    Create(&s);
    if (check_brackets(line)) {
        printf("Не сбалансировано\n");
    }
    else {
        printf("Сбалансировано\n");
    }

    Destroy(&s);
    return 0;
}