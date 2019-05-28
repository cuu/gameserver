#ifndef STD_STACK_H
#define STD_STACK_H

#include "types.h"

typedef struct _Stack {
    int stack[MAX_BUFFERS];
    int top;
} Stack;

int isempty(Stack *s);

int isfull(Stack *s);

int size(Stack *s);

int peek(Stack *s);

int pop(Stack *s);

int push(Stack *s, int data);

#endif