#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "foo.h"

int main() {
    char input_file[256], output_file[256];

    printf("Введите путь к входному файлу: ");
    if (!fgets(input_file, sizeof(input_file), stdin)) return 1;
    input_file[strcspn(input_file, "\n")] = '\0';

    printf("Введите путь к выходному файлу: ");
    if (!fgets(output_file, sizeof(output_file), stdin)) return 1;
    output_file[strcspn(output_file, "\n")] = '\0';

    int output_exists = (access(output_file, F_OK) == 0);
    if (output_exists) {
        Status same_status = same_files(input_file, output_file);
        if (same_status == STATUS_SAME_FILES) {
            printf("Ошибка: входной и выходной файлы совпадают, риск затирания данных.\n");
            return 0;
        }
    }

    LinkedList list_livers = create_list();
    LinkedList undo_stack = create_list();

    Status status = load_file(input_file, &list);
    if (status != STATUS_OK) {
        printf("Ошибка при чтении данных: код %d\n", status);
        delete_list(&list);
        delete_list(&undo_stack);
        return 0;
    }
    printf("Загружено %zu жителей\n", list.size);

     int choice = -1;
    do {
        printf("0. Выход\n");
        printf("1. Поиск жителя с заданными параметрами\n");
        printf("2. Изменить жителя\n");
        printf("3. Добавление жителя\n");
        printf("4. Удаление жителя\n");
        printf("5. Сохранить данные из списка в файл\n");
        printf("6. Отменить N/2 последних действий\n");

        char input[16];
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Ошибка чтения ввода.\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) != 1) {
            printf("Неправильное число, попробуйте ещё раз.\n");
            continue;
        }
        else if (sscanf(input, "%d", &choice) != 1) {
            printf("Неверный ввод.\n");
            continue;
        }
        
        switch (choice) {
            
        }
    } while (choice);

    delete_list(&list);
    delete_list(&undo_stack);
    return 0;
}