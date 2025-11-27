#ifndef INTERPRETATOR_H
#define INTERPRETATOR_H

#include <stdio.h>
#include <stdbool.h>


typedef enum {
    STATUS_OK = 0,
    STATUS_SYNTAX_ERROR,
    STATUS_DIVISION_BY_ZERO,
    STATUS_UNINITIALIZED_VAR,
    STATUS_INVALID_VARIABLE_NAME,
    STATUS_FILE_ERROR,
    STATUS_MEMORY_ERROR,
    STATUS_EOF,
    STATUS_OVERFLOW,
} Status;


// STACK
typedef struct Item Item;  

typedef struct {
    char op;       
    int num;    
    bool is_op;    
} Elem;

struct Item {
    Elem data;
    Item* prev;
};

typedef struct {
    int size;
    Item* top;
} Stack;

void Create(Stack *s);
bool Empty(Stack *s);
int Size(Stack *s);
bool Push(Stack *s, Elem t);
bool Pop(Stack *s);
Elem Top(Stack *s);
void Destroy(Stack *s);


// EVAL EXPRESSION
Elem make_operator(char op);
Elem make_number(double num);
int get_priority(char op);
int apply_op(int a, int b, char op, int *ok);
Status eval(FILE *input, const int vars[26], const int init_vars[26], int *result, int cur_line, int for_print);
Status eval_expression_string(const char *expression_str, const int vars[26], 
                              const int init_vars[26], int *result, int cur_line);

void skip_spaces(FILE *file);

void log_command(FILE *trace, int line_num, const char *command, 
                 const int vars[26], const int init_vars[26], const char *operation_desc);
char* get_vars_state(const int vars[26], const int init_vars[26]);
char* read_command_line_dynamic(FILE *input, Status *status);
Status interpret(FILE *input, FILE *trace);


#endif