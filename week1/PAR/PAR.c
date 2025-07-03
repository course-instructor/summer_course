#include <stdio.h>
#include <string.h>

#include "stack.h"

const char SPETIAL_CHARS[][2] = /* define the different spetial chars with an array of arrais
so that in each one the first char is the opening and the second is the corresponding closing */

{
    {'{', '}'},
    {'[', ']'},
    {'(', ')'},
};

int is_closing(char c, char top)
{
    /**
     * the function checks if the input c is closing the parenthesis at the top of the stack
     * 
     * var c: the char to check if its closing that is closing the parenthesis
     * var top: the char at the top of the stack that is opening the parenthesis
     * 
     * c type: char
     * top type: char
     * 
     * return: 1 if c is the closing char for the top, 0 if not
     * return type: int (0 or 1)
     */

    int ret = 0; // return value
    int i = 0; // index to loop through the special chars array

    while((i < 3) && ret == 0)
    {
        if (c == SPETIAL_CHARS[i][1] && top == SPETIAL_CHARS[i][0]) // check if c is a closing char for the top
        {
            ret = 1; // return true if c is a closing char for top
        }
        i++; // move to the next pair of special chars
    }
    return ret;
}



int in_spetial(char c)
{
    /**
     * the function checks if the input c is one of the special characters
     * 
     * var c: the char to check
     * c type: char
     * 
     * return: 1 if c is an opening char, 2 if c is a closing char, 0 if c is not a special char
     * return type: int (0, 1 or 2)
     */

    int i = 0; // index to loop through the special chars array
    int ret = 0; // return value

    while((i < 3) && ret == 0)// loop through the special chars array
    {
        if (c == SPETIAL_CHARS[i][0]) // check if c is an opening char
        {
            ret = 1; // return 1 if c is an opening char
        }
        else if (c == SPETIAL_CHARS[i][1]) // check if c is a closing char
        {
            ret = 2; // return 2 if c is a closing char

        }
        i++; // move to the next pair of special chars
    }
    return ret; 
}

int check_segment(char s [])
{
    /**
     * the function checks if the string has a problem of parcing the parenthasize
     * by creating a stack and pushing the opening parenthasis into it unless they are inside
     * a comment or a string...
     * 
     * var s: a string to check 
     * 
     * s type: char[] (string)
     * 
     * return: 1 if there are no parcing problems in the string, otherwise 0
     * rtype: int (0,1)
     */
    int ret = 1; // return value initialized to 1 so that if its changed to 0 its easy to exit the loop
    Stack my_stack;
    Stack * stack = &my_stack; //stack that will store the opening of parenthesis, comments and strings
    char top; // the top element of the stack

    initialize(stack);


    int length = strlen(s); // get the length of the string
    for (int i = 0; (i < length) && (ret != 0); i++) // go through each char in the string until the end or the ret changes
    {
        top = peek(stack); // get the top element of the stack

        if(top == '"') // check if the next char is inside a string
        {
            if(s[i] == '"') // if the next char is closing the string
            {
                pop(stack); // pop the opening of the string from the stack
            }
        }

        else if(top == '*')  // check if the next char is inside a comment
        {
            if(s[i] == '*' && (i + 1 < length) && (s[i + 1] == '/')) // check if the next char is closing the comment
            {
                pop(stack); // pop the opening of the comment from the stack
            }
        }

        else if(s[i] == '"') // check if the next char is opening a string
        {
            push(stack, s[i]); // push the opening of the string to the stack
        }

        else if(s[i] == '/' && (i + 1 < length) && s[i + 1] == '*') // check if the next char is opening a comment
        {
            push(stack, '*'); // push the opening of the comment to the stack
            i++; // skip the next char since its part of the comment opening
        }
        else // if not inside a string or comment
        {
            int is_spetial = in_spetial(s[i]); // check if the next char is a special char
            if (is_spetial == 1)// check if the next char is an opening char
            {
                push(stack, s[i]); // push the opening char to the stack
            }
            else if (is_spetial == 2) // check if the next char is a closing char
            {
                if(is_closing(s[i], top)) // check if the closing char is closing the parenthesis at the top of the stack
                {
                    pop(stack); // pop the opening char from the stack
                }

                else // if not closing the parenthesis at the top of the stack
                {
                    ret = 0; // return false
                }
            }
        }
    }

    if(peek(stack) == '*')// check if the stack still has an opening comment
    {
        pop(stack); // pop it since it is legal
    }


    return (ret && isEmpty(stack)); // return 1 if both the stack is empty and there weren't any problems detected in the loop
}

void par(void)
{
    
    char s[100]; // string to hold the input text
    char line[100]; //string to hold each line of the input
    // printf("enter a text to check for parcing problems (up to 100 chars):\n");

    int i = 0; // index to loop through the string

    


    while(fgets(line, 100, stdin) != NULL) // read the input text line by line
    {
        for(int j = 0; (j < 100) && (line[j] != '\n'); j++) // loop through each char in the line
        {
            s[i++] = line[j]; // add the char to the string
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
        s[i++] = '\n'; // add a new line char to the string

    }

    if (check_segment(s) == 0) // check if the whole string has parcing problems
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