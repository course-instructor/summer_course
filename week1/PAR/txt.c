int peek(Stack *stack) {
    // Check if the stack is empty
    if (isEmpty(stack)) {
        
        return -1;
    }
    // Return the top element without removing it
    return stack->arr[stack->top];
}