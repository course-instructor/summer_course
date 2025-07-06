// C Program to demonstrate how to Implement a Stack
#include <stdio.h>
#include <stdbool.h>

#include "stack.h"

typedef struct stack_t {
    int arr[MAX_SIZE];
    int top;
} stack_t;

// Function to initialize the stack
void stack_initialize(stack_t *stack) {
    // Set top index to -1 to indicate an empty stack
    stack->top = -1;  
}

// Function to check if the stack is empty
int stack_isEmpty(stack_t *stack) {
    // If top is -1, the stack is empty
    return stack->top == -1;  
}

// Function to check if the stack is full
int stack_isFull(stack_t *stack) {
    // If top is MAX_SIZE - 1, the stack is full
    return stack->top == MAX_SIZE - 1;  
}

// Function to push an element onto the stack
void stack_push(stack_t *stack, int value) {
    // Check for stack overflow
    if (stack_isFull(stack)) {
// /        printf("Stack Overflow\n");
        return;
    }
    // Increment top and add the value to the top of the stack
    stack->arr[++stack->top] = value;
    // printf("Pushed %d onto the stack\n", value);
}

// Function to pop an element from the stack
int stack_pop(stack_t *stack) {
    // Check for stack underflow
    if (stack_isEmpty(stack)) {
        // printf("Stack Underflow\n");
        return -1;
    }
    // Return the top element 
    int popped = stack->arr[stack->top];
    // decrement top pointer
    stack->top--;
    // printf("Popped %d from the stack\n", popped);
    // return the popped element
    return popped;
}

// Function to peek the top element of the stack
int stack_peek(stack_t *stack) {
    // Check if the stack is empty
    if (stack_isEmpty(stack)) {
        // printf("Stack is empty\n");
        return -1;
    }
    // Return the top element without removing it
    return stack->arr[stack->top];
}

