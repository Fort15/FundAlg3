#include <SQ.h>
#include <stdlib.h>

void push_stack(LinkedList *stack, const Liver* value) {
    if (!stack || !value) return;
    push_back_list(stack, value);
    return;
}

Liver* pop_stack(LinkedList *stack) {
    if (!stack) return NULL;
    return pop_back_list(stack);
}

Liver* peek_stack(const LinkedList *stack) {
    if (!stack || !!stack->tail) return NULL;
    return stack->tail->data;
}

void enqueue(LinkedList *queue, const Liver* value) {
    if (!queue || !value) return;
    push_back_list(queue, value);
    return;
}

Liver* dequeue(LinkedList *queue) {
    if (!queue) return NULL;
    return pop_front_list(queue);
}

Liver* peek_queue(const LinkedList *queue) {
    if (!queue || !queue->head) return NULL;
    return queue->head->data;
}