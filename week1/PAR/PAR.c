#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// Define the maximim capacity of the stack
#define MAX_SIZE 100

// Define a structure for the stack
typedef struct {
    // Array to store stack elements
    int arr[MAX_SIZE];  
    // Index of the top element in the stack
    int top;        
} Stack;

// Function to initialize the stack
void initialize(Stack *stack) {
    // Set top index to -1 to indicate an empty stack
    stack->top = -1;  
}

// Function to check if the stack is empty
bool isEmpty(Stack *stack) {
    // If top is -1, the stack is empty
    return stack->top == -1;  
}

// Function to check if the stack is full
bool isFull(Stack *stack) {
    // If top is MAX_SIZE - 1, the stack is full
    return stack->top == MAX_SIZE - 1;  
}

// Function to push an element onto the stack
void push(Stack *stack, int value) {
    // Check for stack overflow
    if (isFull(stack)) {
        printf("Stack Overflow\n");
        return;
    }
    // Increment top and add the value to the top of the stack
    stack->arr[++stack->top] = value;
    
}

// Function to pop an element from the stack
int pop(Stack *stack) {
    // Check for stack underflow
    if (isEmpty(stack)) {
        printf("Stack Underflow\n");
        return -1;
    }
    // Return the top element 
    int popped = stack->arr[stack->top];
    // decrement top pointer
    stack->top--;
    
    // return the popped element
    return popped;
    
}

// Function to peek the top element of the stack
int peek(Stack *stack) {
    // Check if the stack is empty
    if (isEmpty(stack)) {
        
        return -1;
    }
    // Return the top element without removing it
    return stack->arr[stack->top];
}
void print_balanced()
{
    printf("The line is balanced :)\n");
}
void print_not_balanced()
{
    printf("The line is not balanced :(\n");
}
void print_line(char *line)
{
    printf("Checking line: %s", line);
}
/**
 * @brief the function checks for each line if its sorted as required
 * 
 * @param the function gets a line from a whole string 
 * @return it returns an int but it prints what line is being checked and if its balanced or not
 */
int check_line(char *line)
{
    print_line(line);

    char letter;
    Stack closers;
    initialize(&closers);
    int char_in_line;

    for(char_in_line = 0; line[char_in_line]!='\n' && line[char_in_line]!='\0'; char_in_line++)//we are running till the end of the line
    {
        letter = line[char_in_line];
        if(letter == '(' || letter == '{' || letter == '[')
        {
            push(&closers, letter);// if there is an pener we want to push it to the stack so we can check if the next closer is the same type
        }
        else if(letter == ')' || letter == '}'|| letter == ']')
        {
            if (isEmpty(&closers)) 
            {
                print_not_balanced();// if on the top of the stack there is no opener of the same type its not balanced
                return 0;
            }
            char check = peek(&closers);
            if((letter == ')' && check == '(') ||
               (letter == ']' && check == '[') ||
               (letter == '}' && check == '{'))
            {
                pop(&closers);//if the closer is the same type of opener that is in the top of the stack its fine and we remove it
            }
            else
            {
                print_not_balanced();
                return 0;
            }
        }
        else if(letter == '"')// we want to ignore the strings 
        {
            char_in_line++;
            while (line[char_in_line]!='"')
            {
                char_in_line++;//moving till we are out of the string 
            }
        }
        else if(letter == '/' && line[char_in_line+1] == '*')// we want to ignore the comments
        {
            char_in_line+=2;
            while (!(line[char_in_line] == '*' && line[char_in_line+1] == '/'))
            {
                char_in_line++;// moving till we are out of the comment
            }
            
            
        }
    }
    if(!isEmpty(&closers))// if there is any type of closer in the end of the line it means the line isnt balanced
    {
        print_not_balanced();
        return 0;
    }
    else//if the stack is empty it means the line is balanced
    {
        print_balanced();
        return 1;
    }
}

/**
 * @brief the function checks if the whole string as a unit is balanced
 * 
 * @return it prints if the whole string is balanced or not
 */
int check_full_string()
{
    char string[MAX_SIZE];
    Stack closers_whole_text;
    initialize(&closers_whole_text);

    while (fgets(string, MAX_SIZE, stdin) != NULL) // we want to read until the end
    {
        check_line(string);// for each line we will check if the line for itself is balanced or not

        for (int char_in_string = 0; string[char_in_string] != '\0'; char_in_string++)//moving on the whole string
         {
            char letter = string[char_in_string];
            if(letter == '(' || letter == '{' || letter == '[') 
            {
                push(&closers_whole_text, letter);// if we see an opener we want to push it to the stack to compare it to the next closer we will see
            }
            else if(letter == ')' || letter == '}'|| letter == ']') // if we see the other type of opener in the top of the stack its not balanced,else we continue to check
            {
                char check = peek(&closers_whole_text);
                if(letter == check+1 || letter == check+2) 
                {
                    pop(&closers_whole_text);
                }
                else 
                {
                    printf("The whole text is not balanced!! :(\n");
                    return 0;
                }
            }
            else if(letter == '"')// we want to ignore strings
        {
            char_in_string++;
            while (string[char_in_string]!='"')
            {
                char_in_string++;
            }
        }
        

        }
    }

    if (!isEmpty(&closers_whole_text))// if there is any type of closer in the end of the line it means the line isnt balanced
    {
        printf("The whole text is not balanced!! :(\n");
        return 0;
    } 
    else //if the stack is empty it means the line is balanced
    {
        printf("The whole text is balanced!! :)\n");
        return 0;
    }
}


int main(void)
{
    check_full_string();

}




