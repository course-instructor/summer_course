#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define BRACKET_TYPE_NUM 3
#define QUOTATIONS_NUM 2
#define EOD '\0'
int isTextBalanced(char * text);
void printBalancedText(char * src_text);
void balanced(void);
int getInput(char lines[100][100]);
int main() 
{

    balanced();


    return 0;
}
/*
 * balanced - start the programs and prints messages
*/
void balanced(void){
    char text[100] = "";
    char lines[100][100];
    int line_count = getInput(lines);
    int getInput(char lines[100][100]);
    int i;
    printf("Hello!\n");
    printf("Today we gonna check if you know how to close brackets!!11!!!!\n");
    printf("input text and this program will tell if your text is balanced or not!\n");
    printf("Don't forget - you can enter multiple lines (press ctrl+d when you done writing)\n");
    
    for (i = 0; i < line_count; i++) /* for each line of text */
    {
      printBalancedText(lines[i]); /* print if balanced or unbalanced */
      strcat(text,lines[i]);   /* connect each line to a single string to check for all text overall if its balanced or not */
    } 
    printf("Overall your is text is: ...");
    if(isTextBalanced(text))
    {
      printf("BALANCED !!! :D\n");
    }else
    {
      printf("unbalnced .  D;\n");
    }
    

    
  

}
/**
 * getInput - gets multiple lines of input from user
 * 
 * @lines   - saves each line of input
 * @return  - the number of lines written by the user
 */
int getInput(char lines[100][100]){
    
    int line_count = 0;

    /* this part was kidnapped by me from stackOverflow.com! */
    /* https://stackoverflow.com/questions/25770335/while-loop-asking-for-input-until-ctrl-d-using-c */
    /* keep asking for input until gets EOF (ctrl+d) */
    for ( ;; )
    {
      fflush( NULL );  /* makes sure the input we getting isn't disturbed by the prvious input */
      if ( fgets( lines[line_count], 100,stdin) == NULL ){ 
              break;
      }

      line_count++;
    }
    return line_count;
}


/**
 * printBalancedText - prints a massages next to a line of text which tells if the text balanced or unbalanced
 * 
 * text - the line which is being checked
 */
void printBalancedText(char * text)
{
    
    if(isTextBalanced(text))
    {
        printf("(BALANCED)     %s\n",text);
    }
    else
    {
        printf("(UNBALANCED)   %s\n",text);
    }
    
}



/*
* isTextBalanced - Checks if all brackets are legal (if open brackets are closed afterwards). 
*
*  @text: string with brackets.
*  @return 1 if brackets are legal, 0 if ilegal.
*/
int isTextBalanced(char * text)
{
    /* type of brackets */
    char bracket_open [BRACKET_TYPE_NUM] = {'(','{','['};
    char bracket_close[BRACKET_TYPE_NUM] = {')','}',']'};
    
    int i;

    /* stack */
    Stack s; 
    initialize(&s);

    while(*text != EOD)  
  {
    /* dealing with strings by ignoring input between quotation mark */
    if(*text == '\"')
    {
      push(&s,'\"');           /* quotation mark into the stacks */
      text++;
      while(*text != peek(&s)) /* skips over text until quation marks closed */
      {
        text++;
      }
      pop(&s);                 /* remove quation marks from the stack and normal */
    }

    /* dealing with comments by ignoring input between comments */
    if(*text == '\\' && *(text+1) == '*')
    {
      push(&s,'*'); /* the * symbole represent comment in the stack */
      text++;
      text++;
      while(*text != EOD && *text != '*' && *(text+1) != '\\') /* skips over text until comment is closed */
      {
        text++;
      }
      pop(&s);      /* remove quation marks from the stack and continue */
    }
  

    /* dealing with all types of brackets */
    for (i = 0; i < BRACKET_TYPE_NUM; i++) /* for every type of bracket: */
    {
      /* push into the stack every opening bracket */
      if(*text == bracket_open[i])
      {
          push(&s,*text); 
      }
      else if(*text == bracket_close[i]) /* if the current char is a closing bracket */
      {
        if(pop(&s) != bracket_open[i])   /* then check if closing bracket is of the same type as the top open bracket in the stack */
        {
        
          return 0;   /* if its a diffrent tpy then exit and return unbalanced */
        }
      }
    }
    text++; /* continue to the next char */
  }
  /* if each bracket had the corresponding closing bracket the stack would be empty  
     which means the text is balanced, otherwise the text is unbalanced */
    
  return isEmpty(&s);
}





