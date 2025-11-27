#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "foo.h"

int validate_string(char *buf, size_t buf_size) {
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0'; 
    } else {
        int c;
        while ((c = fgetc(stdin)) != '\n' && c != EOF) {}
        return 0;
    }

    return 1; 
}

Status read_id(unsigned int *id) {
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(buf, sizeof(buf))) {
        return STATUS_INVALID_INPUT;
    }

    char *endptr = NULL;
    long long val = strtoll(buf, &endptr, 10);
    if (*endptr != '\0' || val <= 0 || val > UINT_MAX) return STATUS_INVALID_INPUT;
    *id = (unsigned int)val;
    return STATUS_OK;
}


Status read_day(int *day) {
    unsigned int d;
    Status s = read_id(&d);
    if (s == STATUS_OK && d >= 1 && d <= 31) {
        *day = (int)d;
        return STATUS_OK;
    }
    return STATUS_INVALID_INPUT;
}

Status read_month(int *month) {
    unsigned int m;
    Status s = read_id(&m);
    if (s == STATUS_OK && m >= 1 && m <= 12) {
        *month = (int)m;
        return STATUS_OK;
    }
    return STATUS_INVALID_INPUT;
}

Status read_year(int *year) {
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(buf, sizeof(buf))) {
        return STATUS_INVALID_INPUT;
    }

    char *endptr;
    long y = strtol(buf, &endptr, 10);
    if (*endptr != '\0') {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return STATUS_INVALID_INPUT;
    }
    *year = (int)y;
    return STATUS_OK;
}

Status read_date(Date *date) {
    printf("Дата рождения (дд.мм.гггг): ");
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(buf, sizeof(buf))) {
        return STATUS_INVALID_INPUT;
    }

    int d, m, y;
    if (sscanf(buf, "%d.%d.%d", &d, &m, &y) != 3) return STATUS_INVALID_INPUT;
    if (!is_valid_date(d, y, m)) return STATUS_INVALID_INPUT;

    date->day = d;
    date->month = m;
    date->year = y;
    return STATUS_OK;
}

Status read_choice(int *choice) {
    char buf[10];
    if (!fgets(buf, sizeof(buf), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(buf, sizeof(buf))) {
        return STATUS_INVALID_INPUT;
    }

    char *endptr = NULL;
    long long val = strtoll(buf, &endptr, 10);
    if (*endptr != '\0' || val < INT_MIN || val > INT_MAX) return STATUS_INVALID_INPUT;
    *choice = (int)val;
    return STATUS_OK;
}


Status read_name(char *name, size_t max_len, const char *str) {
    printf("%s", str);
    if (!fgets(name, max_len, stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(name, sizeof(name))) {
        return STATUS_INVALID_INPUT;
    }
    if (strlen(name) == 0) return STATUS_INVALID_INPUT;
    for (const char *p = name; *p; p++) {
        if (!isalpha((unsigned char)*p)) return STATUS_INVALID_INPUT;
    }
    return STATUS_OK;
}

Status read_gender(char *gender) {
    printf("Пол (M/W): ");
    char g[4];
    if (!fgets(g, sizeof(g), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(g, sizeof(g))) {
        return STATUS_INVALID_INPUT;
    }
    if (strlen(g) != 1) return STATUS_INVALID_INPUT;
    *gender = toupper((unsigned char)g[0]);
    if (*gender != 'M' && *gender != 'W') return STATUS_INVALID_INPUT;
    return STATUS_OK;
}

Status read_income(double *income) {
    printf("Средний доход (>=0): ");
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return STATUS_INVALID_INPUT;
    if (!validate_string(buf, sizeof(buf))) {
        return STATUS_INVALID_INPUT;
    }
    char *endptr;
    double val = strtod(buf, &endptr);
    if (*endptr != '\0' || val < 0) return STATUS_INVALID_INPUT;
    *income = val;
    return STATUS_OK;
}

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

    LinkedList list_livers = create_list(liver_copy_func, liver_free_func); 
    LinkedList undo_stack = create_list(undo_action_copy_func, undo_action_free_func);

    Status status = load_file(input_file, &list_livers);
    if (status != STATUS_OK) {
        printf("Ошибка при чтении данных: код %d\n", status);
        delete_list(&list_livers);
        delete_list(&undo_stack);
        return 0;
    }
    printf("Загружено %zu жителей\n", list_livers.size);

    int choice = -1;
    do {
        printf("0. Выход\n");
        printf("1. Поиск жителя с заданными параметрами\n");
        printf("2. Изменить жителя\n");
        printf("3. Добавление жителя\n");
        printf("4. Удаление жителя\n");
        printf("5. Сохранить данные из списка в файл\n");
        printf("6. Отменить N/2 последних действий\n");

        if (read_choice(&choice) != STATUS_OK) { printf("Неверный ввод\n"); continue;}
        
        switch (choice) {
            case 1: {
                // Поиск жителя
                printf("Поиск по: 1.ID 2.Фамилии 3.Имени 4.Отчеству 5.Полу 6.Доходу (диапазон) 7.Дате рождения\n");
                int choice1;
                if (read_choice(&choice1) != STATUS_OK) { printf("Неверный ввод\n"); continue;}

                Node *cur = list_livers.head;
                int found = 0;

                switch (choice1) {
                    case 1: {
                        unsigned int id;
                        printf("Введите id: ");
                        if (read_id(&id) != STATUS_OK) {printf("Неверный id\n"); break;}
                        while (cur) {
                            if (((Liver*)cur->data)->id == id) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %s %s %s %c %.3f\n", l->surname, l->name, l->patronymic,
                                l->gender, l->avg_income);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 2: {
                        char surname[64];
                        if (read_name(surname, sizeof(surname), "Фамилия: ") != STATUS_OK) {printf("Неверная фамилия\n"); break;}
                        while (cur) {
                            if (strcmp(((Liver*)cur->data)->surname, surname) == 0) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %s %s %s\n", l->surname, l->name, l->patronymic);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 3: {
                        char name[64];
                            if (read_name(name, sizeof(name), "Имя: ") != STATUS_OK) {printf("Неверное имя\n"); break;}
                        while (cur) {
                            if (strcmp(((Liver*)cur->data)->name, name) == 0) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %s %s %s\n", l->name, l->surname, l->patronymic);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 4: {
                        char patr[64];
                        if (fgets(patr, sizeof(patr), stdin)) {
                            patr[strcspn(patr, "\n")] = '\0';
                            if (strlen(patr) > 0 && !is_valid_name(patr)) {printf("Неверное отчество\n"); break;}
                        } else break;
                        while (cur) {
                            if (strcmp(((Liver*)cur->data)->patronymic, patr) == 0) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %s %s %s\n", l->patronymic, l->surname, l->name);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 5: {
                        char g;
                        if (read_gender(&g) != STATUS_OK) {printf("Неверный пол\n"); break;}
                        while (cur) {
                            if (((Liver*)cur->data)->gender == g) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %c %s %s\n", l->gender, l->surname, l->name);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 6: {
                        double min_inc, max_inc;
                        printf("Мин. доход: ");
                        if (read_income(&min_inc) != STATUS_OK) {printf("Неверный доход\n"); break;}
                        printf("Макс. доход: ");
                        if (read_income(&max_inc) != STATUS_OK) {printf("Неверный доход\n"); break;}
                        while (cur) {
                            if (((Liver*)cur->data)->avg_income >= min_inc && ((Liver*)cur->data)->avg_income <= max_inc) {
                                found = 1;
                                Liver *l = (Liver*)cur->data;
                                printf("-> %s %.3f\n", l->surname, l->avg_income);
                            }
                            cur = cur->next;
                        }
                        break;
                    }
                    case 7: {
                        printf("Поиск по дате рождения: ");
                        printf("1.Точная дата (дд.мм.гггг) ");
                        printf("2.По году (гггг) ");
                        printf("3.По месяцу (мм) ");
                        printf("4.По дню (дд)\n");

                        int mode;
                        if (read_choice(&mode) != STATUS_OK) {printf("Неправильный ввод\n"); break;}

                        int d = -1, m = -1, y = -1;

                        switch (mode) {
                            case 1: {
                                char input[32];
                                printf("Введите дату (дд.мм.гггг): ");
                                if (!fgets(input, sizeof(input), stdin)) break;
                                input[strcspn(input, "\n")] = '\0';
                                if (sscanf(input, "%d.%d.%d", &d, &m, &y) != 3) {
                                    printf("Неверный формат.\n");
                                    break;
                                }
                                if (!is_valid_date(d, y, m)) {
                                    printf("Некорректная дата.\n");
                                    break;
                                }
                                break;
                            }
                            case 2:
                                printf("Год (гггг): ");
                                if (read_year(&y) != STATUS_OK) {printf("Неправильный ввод\n"); break;}
                                break;
                            case 3:
                                printf("Месяц (1-12): ");
                                if (read_month(&m) != STATUS_OK) {printf("Неправильный ввод\n"); break;}
                                break;
                            case 4:
                                printf("День (1-31): ");
                                if (read_day(&d) != STATUS_OK) {printf("Неправильный ввод\n"); break;}
                                break;
                            default:
                                printf("Неверный режим.\n");
                                break;
                        }

                        // Поиск
                        Node *cur = list_livers.head;
                        int found = 0;
                        while (cur) {
                            Liver *l = (Liver*)cur->data;
                            int match = 1;

                            if (y != -1 && l->birth_date.year != y) match = 0;
                            if (m != -1 && l->birth_date.month + 1 != m) match = 0;
                            if (d != -1 && l->birth_date.day != d) match = 0;

                            if (match) {
                                found = 1;
                                printf("→ %s %s (%02d.%02d.%04d)\n", l->surname, l->name,
                                    l->birth_date.day,
                                    l->birth_date.month,
                                    l->birth_date.year);
                            }
                            cur = cur->next;
                        }
                    }
                    default:
                        printf("Неверный выбор\n");
                        break;
                }
                if (!found) printf("Ничего не найдено.\n");
                break;
            }
            case 2: {
                // Изменения жителя
                unsigned int id;
                printf("Введите ID жителя у которого хотели бы изменить данные: ");
                if (read_id(&id) != STATUS_OK) { printf("Неверный ID.\n"); break; }

                Node *cur = list_livers.head;
                size_t index = 0;
                while(cur && ((Liver*)cur->data)->id != id) { cur = cur->next; index++; } 

                if (!cur) {
                    printf("Житель не найден\n");
                    break;
                }

                Liver *old = liver_copy((Liver*)cur->data); 
                Liver *l_cur = (Liver*)cur->data;

                int choice2;
                printf("Что изменить у жителя? 1.ID 2.Фамилия 3.Имя 4.Отчество 5.ДР 6.Пол 7.Доход\n");
                if (read_choice(&choice2) != STATUS_OK) {printf("Неверный выбор\n"); break;}

                switch(choice2) {
                    case 1: {
                        unsigned int id2;
                        printf("Введите новый id: ");
                        if (read_id(&id2) != STATUS_OK) { printf("Неверный id\n"); liver_free(old); break;}
                        Node *cur2 = list_livers.head;
                        while(cur && ((Liver*)cur->data)->id != id) cur = cur->next;
                        if (!cur) {
                            l_cur->id = id2;
                            UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                            if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                            printf("id обновлён\n");
                        } else {
                            printf("Пользователь с введённым id уже существует\n");
                        }
                    break;
                    }
                    case 2: {
                        char surname[64];
                        if (read_name(surname, sizeof(surname), "Новая фамилия: ") != STATUS_OK) { printf("Неверная фамилия\n"); liver_free(old); break;}
                        strcpy(l_cur->surname, surname);
                        UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                        if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                        printf("Фамилия обновлена");
                        break;
                    }
                    case 3: {
                        char name[64];
                        if (read_name(name, sizeof(name), "Новое имя: ") != STATUS_OK) { printf("Неверное имя\n"); liver_free(old); break;}
                        strcpy(l_cur->name, name);
                        UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                        if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                        printf("Имя обновлено\n");
                        break;
                    }
                    case 4: {
                        printf("Новое отчество: ");
                        char patr[64];
                        if (fgets(patr, sizeof(patr), stdin)) {
                            patr[strcspn(patr, "\n")] = '\0';
                            if (strlen(patr) > 0 && !is_valid_name(patr)) {printf("Неверное отчество\n"); liver_free(old); break;}
                            strcpy(l_cur->patronymic, patr);
                            UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                            if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                            printf("Отчество обновлено.\n");
                        } else {
                            liver_free(old);
                        }
                        break;
                    }
                    case 5: {
                        Date tmp_date = {0};
                        if (read_date(&tmp_date) != STATUS_OK) {
                            printf("Неверна введена дата");
                            liver_free(old);
                        } else {
                            l_cur->birth_date = tmp_date;
                            UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                            if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                            printf("Пол обновлён\n");
                        }
                        break;
                    }
                    case 6: {
                        char new_gender;
                        if (read_gender(&new_gender) != STATUS_OK) {
                            printf("Неверно введён пол\n");
                            liver_free(old);
                        } else {
                            l_cur->gender = new_gender;
                            UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                            if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                            printf("Дата обновлён\n");
                        }
                        break;
                    }
                    case 7: {
                        double new_income;
                        if (read_income(&new_income) != STATUS_OK) {
                            liver_free(old);
                            printf("Неверный доход\n");
                        } else {
                            l_cur->avg_income = new_income;
                            UndoAction *act = create_undo_action(ACTION_UPDATE, index, old, NULL);
                            if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                            printf("Доход обновлён\n");
                        }
                        break;
                    }
                    default: {
                        liver_free(old);
                        printf("Неверный выбор\n");
                    }
                }
                break;
            }
            case 3: {
                // Добавление жителя
                printf("ID: ");
                Liver l = {0};
                if (read_id(&l.id) != STATUS_OK) { printf("Неверный ID.\n"); break; }
                Node *cur2 = list_livers.head;
                while(cur2 && ((Liver*)cur2->data)->id != l.id) cur2 = cur2->next;
                if (cur2) { printf("Пользователь с введённым id уже существует\n"); break; }
                if (read_name(l.surname, sizeof(l.surname), "Фамилия: ") != STATUS_OK) { printf("Неверная фамилия.\n"); break; }
                if (read_name(l.name, sizeof(l.name), "Имя: ") != STATUS_OK) { printf("Неверное имя.\n"); break; }
                printf("Отчество (может быть пустым): ");
                if (!fgets(l.patronymic, sizeof(l.patronymic), stdin)) break;
                l.patronymic[strcspn(l.patronymic, "\n")] = '\0';
                if (strlen(l.patronymic) > 0 && !is_valid_name(l.patronymic)) { printf("Неверное отчество.\n"); break; }
                if (read_date(&l.birth_date) != STATUS_OK) { printf("Неверная дата.\n"); break;}
                if (read_gender(&l.gender) != STATUS_OK) { printf("Неверный пол.\n"); break; }
                if (read_income(&l.avg_income) != STATUS_OK) { printf("Неверный доход.\n"); break; }

                size_t index = 0;
                Node *cur = list_livers.head;
                while (cur && liver_cmp_by_age(&l, (Liver*)cur->data) > 0) {
                    cur = cur->next;
                    index++;
                }
                insert_at_list(&list_livers, index, &l);


                UndoAction *act = create_undo_action(ACTION_INSERT, index, NULL, &l);
                if (act) {push_stack(&undo_stack, act); free_undo_action(act);}
                printf("Житель добавлен.\n");
                break;
            }
            case 4: {
                // Удаление жителя
                unsigned int id;
                printf("Введите ID: ");
                if (read_id(&id) != STATUS_OK) { printf("Неверный ID.\n"); break; }

                Node *cur = list_livers.head;
                size_t index = 0;
                while(cur && ((Liver*)cur->data)->id != id) { cur = cur->next; index++; } 

                if (!cur) {
                    printf("Житель не найден\n");
                    break;
                }

                Liver *old = liver_copy((Liver*)cur->data);
                delete_at_list(&list_livers, index);

                UndoAction *act = create_undo_action(ACTION_DELETE, index, old, NULL);
                if (act) {push_stack(&undo_stack, act); free_undo_action(act);}

                liver_free(old);

                printf("Житель успешно удалён\n");
                break;
            }
            case 0: {
                printf("Выход...\n");
                break;
            }
            case 5: {
                status = save_file(output_file, &list_livers);
                if (status == STATUS_OK) {
                    printf("Список сохранён в файл\n");
                } else {
                    printf("Ошибка сохранения: код %d\n", status);
                }
                break;
            }
            case 6: {
                status = undo_last_actions(&undo_stack, &list_livers);
                if (status == STATUS_OK) {
                    printf("Отменено последние N/2 действий\n");
                } else {
                    printf("Ошибка во время отката действий: код %d", status);
                }
                break;
            }
            default: {
                printf("Неверный выбор\n");
                break;
            }
        }
    } while (choice);

    delete_list(&list_livers);
    delete_list(&undo_stack);
    return 0;
}