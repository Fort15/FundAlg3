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