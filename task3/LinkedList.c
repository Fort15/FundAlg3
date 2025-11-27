#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node* create_node(LinkedList *list, const void* value) {
    if (!list || !value || !list->copy_func) return NULL;
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) return NULL;
    
    n->data = list->copy_func(value); 
    
    if (!n->data) {
        free(n);
        return NULL;
    }
    n->prev = n->next = NULL;
    return n;
}

void destroy_node(LinkedList *list, Node *n) {
    if (n && list && list->free_func) {
        list->free_func(n->data); 
        free(n);
    } else if (n) {
        free(n); 
    }
}

LinkedList create_list(CopyDataFunc copy_func, FreeDataFunc free_func) {
    LinkedList l = {0};
    l.copy_func = copy_func;
    l.free_func = free_func;
    return l;
}

void erase_list(LinkedList *list) {
    if (!list) return;
    while (list->head) {
        Node *n = list->head;
        list->head = n->next;
        destroy_node(list, n); 
    }
    list->tail = NULL;
    list->size = 0;
}

void delete_list(LinkedList *list) {
    if (!list) return;
    erase_list(list);
    // free(list);
}

void push_back_list(LinkedList *list, const void *value) {
    if (!list || !value) return;
    Node *n = create_node(list, value);
    if (!n) return;
    if (!list->head) {
        list->head = list->tail = n;
    } else {
        n->prev = list->tail;
        list->tail->next = n;
        list->tail = n;
    }
    list->size++;
}

void* pop_back_list(LinkedList *list) {
    if (!list || !list->tail) return NULL;
    Node *n = list->tail;
    void *data = n->data;
    if (list->head == list->tail) {
        list->tail = list->head = NULL;
    } else {
        list->tail = n->prev;
        list->tail->next = NULL;
    }
    n->data = NULL;
    free(n);
    list->size--;
    return data;
}


void push_front_list(LinkedList* list, const void* value) {
    if (!list || !value) return;
    Node* n = create_node(list, value);
    if (!n) return;
    if (!list->head) { 
        list->head = list->tail = n;
    } else {
        n->next = list->head;
        list->head->prev = n;
        list->head = n;
    }
    list->size++;
}

void* pop_front_list(LinkedList *list) {
    if (!list || !list->tail) return NULL;
    Node *n = list->head;
    void *data = n->data;
    if (list->head == list->tail) {
        list->tail = list->head = NULL;
    } else {
        list->head = n->next;
        list->head->prev = NULL;
    }
    n->data = NULL;
    free(n);
    list->size--;
    return data;
}

void insert_at_list(LinkedList *list, size_t index, const void *value) {
    if (!list || !value) return;
    if (index > list->size) {printf("Index out of range\n"); return;}

    if (index == 0) {push_front_list(list, value);}
    else if (index == list->size) {push_back_list(list, value);}
    else {
        Node* cur = list->head;
        for (size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        Node* n = create_node(list, value);
        if (!n) return;
        n->prev = cur->prev;
        n->next = cur;
        cur->prev->next = n;
        cur->prev = n;
        list->size++;
    }
}

void delete_at_list(LinkedList* list, size_t index) { 
    if (!list) return;
    if (index > list->size - 1) {printf("Index out of range\n"); return;}

    if (index == 0) {
        void* tmp = pop_front_list(list);
        if (list->free_func && tmp) list->free_func(tmp);
    } else if (index == list->size - 1) {
        void* tmp = pop_back_list(list);
        if (list->free_func && tmp) list->free_func(tmp);
    } else {
        Node* cur = list->head;
        for (size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        destroy_node(list, cur);
        list->size--;
    }
}

void* get_at_list(const LinkedList* list, size_t index) {
    if (!list) return NULL;
    if (index > list->size - 1) {printf("Index out of range\n"); return NULL;}
    Node* cur = list->head;
    for (size_t i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

// только для liver
int is_equal_list(const LinkedList *l1, const LinkedList *l2) {
    if (!l1 || !l2) return 0;
    if (l1->size != l2->size) return 0;
    Node* a = l1->head;
    Node* b = l2->head;
    while (a && b) {
        if (!liver_equal(a->data, b->data)) return 0;
        a = a->next;
        b = b->next;
    }
    return 1;
}

// STACK
void push_stack(LinkedList *stack, const void* value) { 
    if (!stack || !value) return;
    push_back_list(stack, value);
}

void* pop_stack(LinkedList *stack) { 
    if (!stack) return NULL;
    return pop_back_list(stack);
}

void* peek_stack(const LinkedList *stack) {
    if (!stack || !!stack->tail) return NULL;
    return stack->tail->data;
}

// QUEUE
void enqueue(LinkedList *queue, const void* value) {
    if (!queue || !value) return;
    push_back_list(queue, value);
}

void* dequeue(LinkedList *queue) {
    if (!queue) return NULL;
    return pop_front_list(queue);
}

void* peek_queue(const LinkedList *queue) {
    if (!queue || !queue->head) return NULL;
    return queue->head->data;
}