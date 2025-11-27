#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include "Liver.h"

typedef void* (*CopyDataFunc)(const void*); 
typedef void (*FreeDataFunc)(void*);

// UNDO
typedef enum {
    ACTION_INSERT,
    ACTION_DELETE,
    ACTION_UPDATE
} Action;

typedef struct {
    Action action;
    size_t index;
    Liver *old_value;   
    Liver *new_value;   
} UndoAction;


typedef struct Node {     
    void* data;  
    struct Node *prev;     
    struct Node *next;  
} Node;  

typedef struct {     
    Node *head;    
    Node *tail;     
    size_t size;  

    CopyDataFunc copy_func; 
    FreeDataFunc free_func;
} LinkedList;  

Node* create_node(LinkedList *list, const void* value);
void destroy_node(LinkedList *list, Node *n);


LinkedList create_list(CopyDataFunc copy_func, FreeDataFunc free_func);
void erase_list(LinkedList *list);
void delete_list(LinkedList *list);
void push_back_list(LinkedList *list, const void *value);
void push_front_list(LinkedList *list, const void *value);
void* pop_back_list(LinkedList *list);
void* pop_front_list(LinkedList *list);
void insert_at_list(LinkedList *list, size_t index, const void *value);
void delete_at_list(LinkedList *list, size_t index);
void* get_at_list(const LinkedList *list, size_t index);
int is_equal_list(const LinkedList *l1, const LinkedList *l2);

// STACK
void push_stack(LinkedList *stack, const void* value);  
void* pop_stack(LinkedList *stack);  
void* peek_stack(const LinkedList *stack); 
 
// QUEUE
void enqueue(LinkedList *queue, const void* value);  
void* dequeue(LinkedList *queue);  
void* peek_queue(const LinkedList *queue);


#endif