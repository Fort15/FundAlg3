#include "interp.h" 
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Проверка правильного количества аргументов: 
    // ./interpreter <input_file> <trace_file>
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <trace_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[1];
    const char *trace_filename = argv[2];

    // Открытие входного файла с командами
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Открытие файла трассировки для записи (файл будет создан или перезаписан)
    FILE *trace_file = fopen(trace_filename, "w");
    if (!trace_file) {
        perror("Error opening trace file");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    // Запуск интерпретатора
    Status status = interpret(input_file, trace_file);

    // Закрытие файлов
    fclose(input_file);
    fclose(trace_file);

    // Отчет о завершении
    if (status == STATUS_OK) {
        printf("Interpretation successful. Print output was written to stdout. Trace written to %s\n", trace_filename);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "\n--- INTERPRETATION FAILED ---\n");
        fprintf(stderr, "Interpretation failed with status code %d.\n", status);
        return EXIT_FAILURE;
    }
}