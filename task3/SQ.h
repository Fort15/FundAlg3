//STACK AND QUEUE
#ifndef SQ_H
#define SQ_H

#include "LinkedList.h"

void push_stack(LinkedList *stack, const Liver* value);  
Liver* pop_stack(LinkedList *stack);  
Liver* peek_stack(const LinkedList *stack); 
 
void enqueue(LinkedList *queue, const Liver* value);  
Liver* dequeue(LinkedList *queue);  
Liver* peek_queue(const LinkedList *queue);

#endif