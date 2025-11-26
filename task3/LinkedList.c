#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

Node* create_node(const Liver *value) {
    if (!value) return NULL;
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) return NULL;
    n->data = liver_copy(value);
    if (!n->data) {
        free(n);
        return NULL;
    }
    n->prev = n->next = NULL;
    return n;
}

void destroy_node(Node *n) {
    if (n) {
        liver_free(n->data);
        free(n);
    }
}

LinkedList create_list(void) {
    LinkedList l = {0};
    return l;
}

void erase_list(LinkedList *list) {
    if (!list) return;
    while (list->head) {
        Node *n = list->head;
        list->head = n->next;
        destroy_node(n);
    }
    list->tail = NULL;
    list->size = 0;
}

void delete_list(LinkedList *list) {
    if (!list) return;
    erase_list(list);
    free(list);
}

void push_back_list(LinkedList *list, const Liver *value) {
    if (!list || !value) return;
    Node *n = create_node(value);
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

Liver* pop_back_list(LinkedList *list) {
    if (!list || !list->tail) return NULL;
    Node *n = list->tail;
    Liver *data = n->data;
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


void push_front_list(LinkedList* list, const Liver* value) {
    if (!list || !value) return;
    Node* n = create_node(value);
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

Liver* pop_front_list(LinkedList *list) {
    if (!list || !list->tail) return NULL;
    Node *n = list->head;
    Liver *data = n->data;
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

void insert_at_list(LinkedList *list, size_t index, const Liver *value) {
    if (!list || !value) return;
    if (index > list->size) {printf("Index out of range\n"); return;}

    if (index == 0) {push_front_list(list, value);}
    else if (index == list->size - 1) {push_back_list(list, value);}
    else {
        Node* cur = list->head;
        for (size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        Node* n = create_node(value);
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
    if (index > list->size) {printf("Index out of range\n"); return;}

    if (index == 0) {
        Liver* tmp = pop_front_list(list);
        liver_free(tmp);
    } else if (index == list->size - 1) {
        Liver* tmp = pop_back_list(list);
        liver_free(tmp);
    } else {
        Node* cur = list->head;
        for (size_t i = 0; i < index; ++i) {
            cur = cur->next;
        }
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        liver_free(cur->data);
        free(cur);
        list->size--;
    }
}

Liver* get_at_list(const LinkedList* list, size_t index) {
    if (!list) return;
    if (index > list->size) {printf("Index out of range\n"); return;}
    Node* cur = list->head;
    for (size_t i = 0; i < index; ++i) {
        cur = cur->next;
    }
    return cur->data;
}

int is_equal_list(const LinkedList *l1, const LinkedList *l2) {
    if (!l1 || !l2) return;
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
