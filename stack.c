#include "stack.h"
#include "node.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

//This function is responsible for allocating memory for any created stack. This code is provided by the assignment pdf.
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

//This function is responsible for freeing the allocated memory of a stack. This code is provided by the assignment pdf.
void stack_delete(Stack **s) {
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

//This function returns the top of the stack which would be the stack size.
uint32_t stack_size(Stack *s) {
    return s->top;
}

//This function checks if the stack is empty and returns true if it is and false otherwise.
bool stack_empty(Stack *s) {
    if (stack_size(s) == 0) {
        return true;
    }
    return false;
}

//This function checks if the stack is full and returns true if it is and false otherwise.
bool stack_full(Stack *s) {
    if (stack_size(s) == s->capacity) {
        return true;
    }
    return false;
}

//This function is responsible pushing a node onto a stack. Returns true if it is succesful and returns false otherwise.
bool stack_push(Stack *s, Node *n) {
    if (!stack_full(s)) {
        s->items[s->top] = n;
        s->top += 1;
        return true;
    }
    return false;
}

//This function is responsible for popping a node from a stack. Returns true if it is successful and returns false otherwise.
bool stack_pop(Stack *s, Node **n) {
    if (!stack_empty(s)) {
        s->top -= 1;
        *n = s->items[s->top];
        return true;
    }
    return false;
}

