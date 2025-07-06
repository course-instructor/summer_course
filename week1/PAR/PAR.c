#include <stdio.h>
#include <string.h>

#include "stack.h"

const char G_SPETIAL_CHARS[][2] = /* define the different spetial chars with an array of arrais
so that in each one the first char is the opening and the second is the corresponding closing */

{
    {'{', '}'},
    {'[', ']'},
    {'(', ')'},
};

int is_closing(char c, char top)
{
    /**
     * @brief the function checks if the input c is closing the parenthesis at the top of the stack 
     * @param c the char to check if its closing the parenthesis
     * @param top the char at the top of the stack that is opening the parenthesis
     * @return 1 if c is the closing char for the top, 0 if not
     */
    
    int ret = 0; // return value
    int index = 0; // index to loop through the special chars array

    while((index < 3) && ret == 0)
    {
        if (c == G_SPETIAL_CHARS[index][1] && top == G_SPETIAL_CHARS[index][0]) // check if c is a closing char for the top
        {
            ret = 1; // return true if c is a closing char for top
        }
        index++; // move to the next pair of special chars
    }
    return ret;
}



int in_spetial(char c)
{
    /**
     * @brief the function checks if the input c is one of the special characters
     * @param c the char to check if its a special char
     * @return 1 if c is an opening char, 2 if c is a closing char, 0 if c is not a special char
     */
    

    int index = 0; // index to loop through the special chars array
    int ret = 0; // return value

    while((index < 3) && ret == 0)// loop through the special chars array
    {
        if (c == G_SPETIAL_CHARS[index][0]) // check if c is an opening char
        {
            ret = 1; // return 1 if c is an opening char
        }
        else if (c == G_SPETIAL_CHARS[index][1]) // check if c is a closing char
        {
            ret = 2; // return 2 if c is a closing char

        }
        index++; // move to the next pair of special chars
    }
    return ret; 
}

int check_segment(char inp_string [])
{
    /**
     * @brief the function checks if the string has a problem of parcing the parenthasize not including ones that are in comments or strings
     * @param inp_string a string to check for parcing problems
     * @return 1 if there are no parcing problems in the string, otherwise 0
     */
    
    int ret = 1; // return value initialized to 1 so that if its changed to 0 its easy to exit the loop
    stack_t my_stack;
    stack_t * stack = &my_stack; //stack that will store the opening of parenthesis, comments and strings
    char top; // the top element of the stack

    stack_initialize(stack);


    int length = strlen(inp_string); // get the length of the string
    for (int index = 0; (index < length) && (ret != 0); index++) // go through each char in the string until the end or the ret changes
    {
        top = stack_peek(stack); // get the top element of the stack

        if(top == '"') // check if the next char is inside a string
        {
            if(inp_string[index] == '"') // if the next char is closing the string
            {
                stack_pop(stack); // pop the opening of the string from the stack
            }
        }

        else if(top == '*')  // check if the next char is inside a comment
        {
            if(inp_string[index] == '*' && (index + 1 < length) && (inp_string[index + 1] == '/')) // check if the next char is closing the comment
            {
                stack_pop(stack); // pop the opening of the comment from the stack
            }
        }

        else if(inp_string[index] == '"') // check if the next char is opening a string
        {
            stack_push(stack, inp_string[index]); // push the opening of the string to the stack
        }

        else if(inp_string[index] == '/' && (index + 1 < length) && inp_string[index + 1] == '*') // check if the next char is opening a comment
        {
            stack_push(stack, '*'); // push the opening of the comment to the stack
            index++; // skip the next char since its part of the comment opening
        }
        else // if not inside a string or comment
        {
            int is_spetial = in_spetial(inp_string[index]); // check if the next char is a special char
            if (is_spetial == 1)// check if the next char is an opening char
            {
                stack_push(stack, inp_string[index]); // push the opening char to the stack
            }
            else if (is_spetial == 2) // check if the next char is a closing char
            {
                if(is_closing(inp_string[index], top)) // check if the closing char is closing the parenthesis at the top of the stack
                {
                    stack_pop(stack); // pop the opening char from the stack
                }

                else // if not closing the parenthesis at the top of the stack
                {
                    ret = 0; // return false
                }
            }
        }
    }

    if(stack_peek(stack) == '*')// check if the stack still has an opening comment
    {
        stack_pop(stack); // pop it since it is legal
    }


    return (ret && stack_isEmpty(stack)); // return 1 if both the stack is empty and there weren't any problems detected in the loop
}

void par(void)
{
    /**
     * @brief the function reads a text from the user and checks for parcing problems in it
     */
    
    char inp_string[100]; // string to hold the input text
    char line[100]; //string to hold each line of the input
    // printf("enter a text to check for parcing problems (up to 100 chars):\n");

    int index = 0; // index to loop through the string

    


    while(fgets(line, 100, stdin) != NULL) // read the input text line by line
    {
        for(int j = 0; (j < 100) && (line[j] != '\n'); j++) // loop through each char in the line
        {
            inp_string[index++] = line[j]; // add the char to the string
        }

        int check = check_segment(line); //check if the line has parcing problems

        if(check == 0) // if there are parcing problems
        {
            printf("parcing problems detected in the line: \n %s\n\n", line); // print the line with problems
        }
        else // if there are no parcing problems
        {
            printf("no parcing problems detected in the line: \n %s\n\n", line); // print the line without problems
        }
        inp_string[index++] = '\n'; // add a new line char to the string

    }

    if (check_segment(inp_string) == 0) // check if the whole string has parcing problems
    {
        printf("the text as a hole HAS parcing problems.\n"); 
    }
    else // if there are no parcing problems
    {
        printf("the text as a hole DOESN'T HAVE parcing problems.\n"); 
    }
    
    
}

int main()
{
    par(); 
    return 0;
}