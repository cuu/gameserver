#include "std_stack.h"

int isempty(Stack *s) {
    if (s->top == -1) return 1;
    else return 0;
}

int isfull(Stack *s) {
    if ((s->top + 1) == MAX_BUFFERS) return 1;
    else return 0;
}

int size(Stack *s) {
    return s->top + 1;
}

int peek(Stack *s) {
    if (!isempty(s))
        return s->stack[s->top];
    else
        return -1;
}

int pop(Stack *s) {
    int data;

    if (!isempty(s)) {
        data = s->stack[s->top];
        s->top = s->top - 1;
        return data;
    } else {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

int push(Stack *s, int data) {

    if (!isfull(s)) {
        s->top = s->top + 1;
        s->stack[s->top] = data;
    } else {
        printf("Could not insert data, Stack is full.\n");
    }
}
