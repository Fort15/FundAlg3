#include "interp.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>


void Create(Stack *s) {
    s->top = NULL;
    s->size = 0;
}

bool Empty(Stack *s) {
    return s->top == NULL;
}

int Size(Stack *s) {
    return s->size;
}

bool Push(Stack *s, Elem t) {
    Item* i = (Item*)malloc(sizeof(Item));
    if (!i) return false;
    i->data = t;
    i->prev = s->top;
    s->top = i;
    s->size++;
    return true;
}

bool Pop(Stack *s) {
    if (Empty(s)) return false;

    Item* i = s->top;
    s->top = s->top->prev;
    s->size--;
    free(i);
    return true;
}

Elem Top(Stack *s) {
    if (s->top) return s->top->data;
    Elem empty = {0};
    return empty;
}

void Destroy(Stack *s) {
    while (s->top) {
        Item* i = s->top;
        s->top = s->top->prev;
        free(i);
    }
    s->top = NULL;
    s->size = 0;
}


void skip_spaces(FILE *file) {
    char c;
    while ((c = fgetc(file)) == ' ');  
    if (c != EOF) ungetc(c, file);  
}

// EVAL expr

Elem make_operator(char op) {
    Elem e;
    e.op = op;
    e.is_op = true;
    return e;
}

Elem make_number(double num) {
    Elem e;
    e.num = num;
    e.is_op = false;
    return e;
}

int get_priority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int int_pow(int base, int exp, int *ok) {
    // Используем long long для безопасных промежуточных вычислений
    long long res = 1;
    long long b = base;
    *ok = 1;

    if (exp < 0) {
        *ok = -2; // Недопустимая степень
        return 0;
    }
    if (exp == 0) return 1;
    if (base == 0) return 0;

    // Флаг для корректной обработки знака
    bool is_negative_result = (base < 0) && (exp % 2 != 0);
    if (base < 0) {
        b = -b; // Работаем с абсолютным значением основания
    }
    
    // Алгоритм быстрого возведения в степень
    while (exp > 0) {
        if (exp & 1) { // Если exp нечетно
            // Проверка переполнения при умножении res * b
            if (b > 0) {
                if (res > INT_MAX / b) { 
                    *ok = 0; 
                    return 0;
                }
            } 
            res *= b;
        }
        
        // Проверка переполнения при возведении в квадрат b * b
        if (exp > 1) { // Проверяем только если b будет использоваться для следующего шага
            if (b > INT_MAX / b) {
                *ok = 0; 
                return 0;
            }
            b *= b;
        }

        exp >>= 1; // exp = exp / 2
    }
    
    // Применение знака
    if (is_negative_result) {
        // Проверка переполнения при преобразовании в отрицательное (для INT_MIN)
        if (res > (long long)INT_MAX + 1) { // (INT_MAX + 1) == |INT_MIN|
            *ok = 0;
            return 0;
        }
        res = -res;
    }

    // Финальная проверка (должно быть в INT, если все предыдущие проверки прошли)
    if (res > INT_MAX || res < INT_MIN) {
        *ok = 0;
        return 0;
    }

    return (int)res;
}


int apply_op(int a, int b, char op, int *ok) {
    *ok = 1; // 1: OK, 0: Overflow, -1: Division by zero

    switch (op) {
        case '+': {
            // Проверка переполнения для сложения через long long
            long long res_ll = (long long)a + b;
            if (res_ll > INT_MAX || res_ll < INT_MIN) {
                *ok = 0;
                return 0;
            }
            return (int)res_ll;
        }
        case '-': {
            // Проверка переполнения для вычитания через long long
            long long res_ll = (long long)a - b;
            if (res_ll > INT_MAX || res_ll < INT_MIN) {
                *ok = 0;
                return 0;
            }
            return (int)res_ll;
        }
        case '*': {
            // Проверка переполнения для умножения через long long
            long long res_ll = (long long)a * b;
            if (res_ll > INT_MAX || res_ll < INT_MIN) {
                *ok = 0;
                return 0;
            }
            return (int)res_ll;
        }
        case '/': {
            if (b == 0) {
                *ok = -1; // Деление на ноль
                return 0;
            }
            // Проверка на переполнение при INT_MIN / -1
            if (a == INT_MIN && b == -1) {
                *ok = 0;
                return 0;
            }
            return a / b;
        }
        case '^': {
            // Используем безопасную функцию возведения в степень
            return int_pow(a, b, ok);
        }
        default:
            *ok = -2; // Неизвестная операция
            return 0;
    }
}

Status eval(FILE *input, const int vars[26], const int init_vars[26], int *result, int cur_line, int for_print) {
    Stack ops, values;
    Create(&ops);
    Create(&values);

    int open_brackets = 0;
    
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (isspace(c)) continue;

        if (isdigit(c)) {
            int num = c - '0';
            while ((c = fgetc(input)) != EOF && isdigit(c)) {
                if (num > (INT_MAX - c + '0') / 10) {
                    Destroy(&ops);
                    Destroy(&values);
                    return STATUS_OVERFLOW;
                }
                num = num * 10 + (c - '0');
            }
            if (c != EOF) ungetc(c, input);
            Push(&values, make_number(num));
        }
        else if (c >= 'A' && c <= 'Z') {
            char var = c; 

            c = fgetc(input);
            if (!(isspace(c) || c == EOF || c == '+' || c == '-' || 
                  c == '*' || c == '/' || c == '^' || c == '(' || c == ')')) {
                printf("Invalid syntax in line %d\n", cur_line);
                Destroy(&ops);
                Destroy(&values);
                return STATUS_SYNTAX_ERROR;
            } 
            ungetc(c, input);

            int index = var - 'A';
            if (init_vars[index]) {
                Push(&values, make_number(vars[index]));
            } else {
                printf("Using not initializated var in line %d\n", cur_line);
                Destroy(&ops);
                Destroy(&values);
                return STATUS_UNINITIALIZED_VAR;
            }
        }
        else if (c == '(') {
            open_brackets++;
            Push(&ops, make_operator('('));
        }
        else if (c == ')') {
            while (!Empty(&ops) && Top(&ops).op != '(') {
                char op = Top(&ops).op; Pop(&ops);
                double b = Top(&values).num; Pop(&values);
                double a = Top(&values).num; Pop(&values);
                int ok;
                int res = apply_op(a, b, op, &ok);
                if (!ok) {
                    Destroy(&ops);
                    Destroy(&values);
                    printf("Error in eval expression in line %d\n", cur_line);
                    return STATUS_DIVISION_BY_ZERO;
                }
                Push(&values, make_number(res));
            }
            if (!Empty(&ops) && Top(&ops).op == '(') Pop(&ops);
            else {
                Destroy(&ops);
                Destroy(&values);
                printf("Error in expression in line %d\n", cur_line);
                return STATUS_SYNTAX_ERROR;
            }
            open_brackets--;
            if (open_brackets == 0 && for_print) {
                skip_spaces(input);
                c = fgetc(input);
                if (c != '\n' || c != EOF) {
                    Destroy(&ops);
                    Destroy(&values);
                    printf("Error in print in line %d\n", cur_line);
                    return STATUS_SYNTAX_ERROR;
                }
                ungetc(c, input);
                break;
            }
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') { 
            while (!Empty(&ops) && get_priority(Top(&ops).op) >= get_priority(c)) {
                char op = Top(&ops).op; Pop(&ops);
                double b = Top(&values).num; Pop(&values);
                double a = Top(&values).num; Pop(&values);
                int ok;
                int res = apply_op(a, b, op, &ok);
                if (!ok) {
                    Destroy(&ops);
                    Destroy(&values);
                    printf("Error in eval expression in line %d\n", cur_line);
                    return STATUS_DIVISION_BY_ZERO;
                }
                Push(&values, make_number(res));
            }
            Push(&ops, make_operator(c));
        }
        else if (c == '\n') {
            ungetc(c, input);
            break;
        } else {
            printf("Invalid syntax in line %d\n", cur_line);
            Destroy(&ops);
            Destroy(&values);
            return STATUS_SYNTAX_ERROR;
        }
    }
    while (!Empty(&ops)) {
        if (Top(&ops).op == '(') {
            printf("Not balanced brakets in line %d\n", cur_line);
            Destroy(&ops);
            Destroy(&values);
            return STATUS_SYNTAX_ERROR;
        }
        char op = Top(&ops).op; Pop(&ops);
        double b = Top(&values).num; Pop(&values);
        double a = Top(&values).num; Pop(&values);
        int ok;
        int res = apply_op(a, b, op, &ok);
        if (!ok) {
            Destroy(&ops);
            Destroy(&values);
            printf("Error in eval expression in line %d\n", cur_line);
            return STATUS_DIVISION_BY_ZERO;
        }
        Push(&values, make_number(res));
    }
    if (Empty(&values) || Size(&values) != 1) {
        printf("Invalid expression in line %d\n", cur_line);
        Destroy(&ops);
        Destroy(&values);
        return STATUS_SYNTAX_ERROR;
    }

    *result = Top(&values).num;
    Destroy(&ops);
    Destroy(&values);
    return STATUS_OK;
}

Status eval_expression_string(const char *expression_str, const int vars[26], 
                              const int init_vars[26], int *result, int cur_line) {
    
    FILE *tmp_file = tmpfile(); // Создает временный файл, который будет удален при fclose
    if (!tmp_file) return STATUS_FILE_ERROR;
    
    // Записываем выражение во временный файл. Добавляем '\n', чтобы eval знал, где закончить чтение.
    if (fprintf(tmp_file, "%s\n", expression_str) < 0) { 
        fclose(tmp_file);
        return STATUS_FILE_ERROR;
    }
    
    rewind(tmp_file); 
    
    // Вызываем оригинальный eval, передавая временный файл
    Status s = eval(tmp_file, vars, init_vars, result, cur_line, 0); 
    
    fclose(tmp_file); 
    
    return s;
}


// Interpretator

char* read_command_line_dynamic(FILE *input, Status *status) {
    size_t capacity = 100;
    size_t length = 0;
    char *buffer = (char*)malloc(capacity);
    if (!buffer) { *status = STATUS_MEMORY_ERROR; return NULL; }
    
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *new_buffer = (char*)realloc(buffer, capacity + 1); // +1 для надежности
            if (!new_buffer) { 
                free(buffer); 
                *status = STATUS_MEMORY_ERROR; 
                return NULL; 
            }
            buffer = new_buffer;
        }
        
        buffer[length++] = (char)c;
        
        if (c == '\n') break;
    }
    
    if (length == 0 && c == EOF) {
        free(buffer);
        *status = STATUS_EOF; 
        return NULL;
    }
    
    buffer[length] = '\0';
    *status = STATUS_OK;
    return buffer; // Вызывающая функция должна вызвать free(buffer)
}

// Функция для генерации строки состояния переменных (A=2, B=3, ...)
char* get_vars_state(const int vars[26], const int init_vars[26]) {
    // Используем статический буфер. 512 байт достаточно для 26 переменных.
    static char buffer[512]; 
    buffer[0] = '\0';
    char temp[32];
    int first = 1;
    
    for (int i = 0; i < 26; i++) {
        if (init_vars[i]) {
            if (!first) {
                strcat(buffer, ", ");
            }
            // Формат: C=value
            snprintf(temp, sizeof(temp), "%c=%d", 'A' + i, vars[i]);
            strcat(buffer, temp);
            first = 0;
        }
    }
    return buffer;
}

// Функция для записи записи в файл трассировки
void log_command(FILE *trace, int line_num, const char *command, 
                 const int vars[26], const int init_vars[26], const char *operation_desc) {
    char clean_command[1024];
    
    // 1. Копирование и очистка команды (удаление пробелов и символа новой строки в конце)
    strncpy(clean_command, command, sizeof(clean_command) - 1);
    clean_command[sizeof(clean_command) - 1] = '\0';
    size_t len = strlen(clean_command);
    while (len > 0 && isspace(clean_command[len - 1])) {
        clean_command[--len] = '\0';
    }
    
    char *vars_state = get_vars_state(vars, init_vars);

    // 2. Запись в файл трассировки в требуемом формате
    // [N] COMMAND      | VARS_STATE | DESCRIPTION
    fprintf(trace, "[%d] %-30s | %s | %s\n", 
            line_num, clean_command, vars_state, operation_desc);
    fflush(trace); 
}

Status interpret(FILE *input, FILE *trace) {
    if (!input || !trace) return STATUS_FILE_ERROR;

    int vars[26];
    int init_vars[26] = {0}; 
    int cur_line = 1;
    Status s = STATUS_OK;

    while (s != STATUS_EOF) {
        
        Status temp_status;
        char *command_line = read_command_line_dynamic(input, &temp_status);
        s = temp_status;
        
        if (s == STATUS_EOF) break;
        if (s != STATUS_OK) return s;

        // 1. Пропускаем начальные пробелы в считанной строке
        char *p = command_line;
        while (isspace(*p) && *p != '\n') p++;

        // Пропускаем пустые строки
        if (*p == '\0' || *p == '\n') {
            free(command_line);
            cur_line++;
            continue;
        }

        char *operation_desc = NULL;
        s = STATUS_OK; // Сбрасываем статус перед выполнением команды
        
        // --- ПАРСИНГ КОМАНДЫ ---

        // 1. ПРИСВАИВАНИЕ (Assignment): A = Expression
        if (*p >= 'A' && *p <= 'Z') {
            char var = *p;
            int index = var - 'A';
            
            char *eq_sign = strchr(command_line, '=');
            
            // Проверка синтаксиса: должно быть 'VAR = ...'
            if (eq_sign && eq_sign > p) { 
                
                // Проверяем, что между переменной и '=' только пробелы
                char *check = p + 1;
                while(check < eq_sign) {
                    if (!isspace(*check)) {
                        s = STATUS_SYNTAX_ERROR;
                        break;
                    }
                    check++;
                }

                if (s == STATUS_OK) {
                    char *expression_str = eq_sign + 1;
                    int result;
                    
                    // Удаляем начальные пробелы в выражении
                    while(isspace(*expression_str)) expression_str++;

                    s = eval_expression_string(expression_str, vars, init_vars, &result, cur_line);
                    
                    if (s == STATUS_OK) {
                        vars[index] = result;
                        init_vars[index] = 1; 
                        
                        // Определяем тип операции для лога
                        if (strchr(expression_str, '+') || strchr(expression_str, '-') || 
                            strchr(expression_str, '*') || strchr(expression_str, '/') || 
                            strchr(expression_str, '^')) {
                            operation_desc = "Arithmetic operation";
                        } else {
                            operation_desc = "Assignment"; // Простое присваивание (например, A = B или A = 5)
                        }

                        // ЛОГИРОВАНИЕ УСПЕШНОЙ ОПЕРАЦИИ
                        log_command(trace, cur_line, command_line, vars, init_vars, operation_desc);
                    }
                }
            } else {
                s = STATUS_SYNTAX_ERROR;
            }
        } 
        
        // 2. ВЫВОД (PRINT): print(Expression)
        else if (strncmp(p, "print", 5) == 0) {
            char *start_expr = p + 5;
            while(isspace(*start_expr)) start_expr++;
            
            if (*start_expr == '(') {
                start_expr++; // Пропускаем '('
                
                // Находим закрывающую ')'
                char *end_paren = strrchr(command_line, ')'); 
                
                if (end_paren && end_paren > start_expr) {
                    
                    // Проверяем, что после ')' нет посторонних символов
                    char *check_end = end_paren + 1;
                    while(*check_end != '\0') {
                        if (!isspace(*check_end) && *check_end != '\n') {
                            s = STATUS_SYNTAX_ERROR;
                            break;
                        }
                        check_end++;
                    }

                    if (s == STATUS_OK) {
                        
                        // Удаляем начальные пробелы в выражении для чистого лога
                        char *temp_expr = start_expr;
                        while(isspace(*temp_expr)) temp_expr++;
                        
                        // Сохраняем строку выражения для лога, учитывая, что start_expr
                        // и end_paren могут указывать на общий буфер command_line.
                        char print_expression_copy[1024];
                        size_t expr_len = end_paren - temp_expr;

                        if (expr_len > sizeof(print_expression_copy) - 1) {
                             expr_len = sizeof(print_expression_copy) - 1;
                        }

                        strncpy(print_expression_copy, temp_expr, expr_len);
                        print_expression_copy[expr_len] = '\0';

                        // Временно обрезаем строку для передачи в eval
                        *end_paren = '\0'; 
                        
                        int result;
                        s = eval_expression_string(start_expr, vars, init_vars, &result, cur_line);
                        
                        *end_paren = ')'; // Восстанавливаем ')'

                        if (s == STATUS_OK) {
                             printf("%d\n", result); // Вывод результата
                             
                             // Динамическое формирование строки описания для лога
                             char log_desc_buffer[1024 + 10]; // Достаточно для "Print " + выражения
                             snprintf(log_desc_buffer, sizeof(log_desc_buffer), "Print %s", print_expression_copy);
                             
                             // ЛОГИРОВАНИЕ УСПЕШНОЙ ОПЕРАЦИИ
                             log_command(trace, cur_line, command_line, vars, init_vars, log_desc_buffer);
                        }
                    }
                } else {
                    s = STATUS_SYNTAX_ERROR;
                }
            } else {
                s = STATUS_SYNTAX_ERROR;
            }
        }
        
        // 3. Неизвестная команда
        else {
            s = STATUS_SYNTAX_ERROR;
        }

        // Общая обработка ошибок
        if (s != STATUS_OK) {
            if (s != STATUS_EOF) {
                // Сообщения о STATUS_UNINITIALIZED_VAR и STATUS_DIVISION_BY_ZERO уже выводит eval
                if (s == STATUS_SYNTAX_ERROR) {
                     printf("Invalid syntax in line %d: %s", cur_line, command_line);
                }
                else if (s == STATUS_OVERFLOW) { // <-- НОВАЯ ОБРАБОТКА ОШИБКИ ПЕРЕПОЛНЕНИЯ
                     printf("Error: Arithmetic overflow or literal value out of int range in line %d\n", cur_line);
                }
            }
            free(command_line);
            return s;
        }
        
        free(command_line);
        cur_line++;
    }
    
    return STATUS_OK;
}