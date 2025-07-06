#pragma once

#define MAX_SIZE 100

typedef struct stack_t stack_t;

void stack_initialize(stack_t *stack);
int stack_isEmpty(stack_t *stack);
int stack_isFull(stack_t *stack);
void stack_push(stack_t *stack, int value);
int stack_pop(stack_t *stack);
int stack_peek(stack_t *stack);
