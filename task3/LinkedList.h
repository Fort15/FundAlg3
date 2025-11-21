#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include "Liver.h"



typedef struct Node {     
    Liver* data;  
    struct Node *prev;     
    struct Node *next;  
} Node;  
typedef struct {     
    Node *head;    
    Node *tail;     
    size_t size;  
} LinkedList;  

Node* create_node(const Liver *value);
void destroy_node(Node *n);

LinkedList create_list(void);
void erase_list(LinkedList *list);
void delete_list(LinkedList *list);
void push_back_list(LinkedList *list, const Liver *value);
void push_front_list(LinkedList *list, const Liver *value);
Liver* pop_back_list(LinkedList *list);
Liver* pop_front_list(LinkedList *list);
void insert_at_list(LinkedList *list, size_t index, const Liver *value);
void delete_at_list(LinkedList *list, size_t index);
Liver* get_at_list(const LinkedList *list, size_t index);
int is_equal_list(const LinkedList *l1, const LinkedList *l2);

#endif