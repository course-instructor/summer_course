#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "NUMBERS.h"    
#define NUM_OF_UNIQUE_NUMBER_WORDS 28
#define MAX_WORD_LENGTH 13
/*Array of each uniqe name for a number in english from 0 - 99*/
static const char *number_names[NUM_OF_UNIQUE_NUMBER_WORDS] = {
   "zero",
   "one",
   "two",
   "three",
   "four",
   "five",
   "six",
   "seven",
   "eight",
   "nine",
   "ten",
   "eleven",
   "twelve",
   "thirteen",
   "fourteen",
   "fifteen",
   "sixteen",
   "seventeen",
   "eighteen",
   "nineteen",
   "twenty",
   "thirty",
   "forty",
   "fifty",
   "sixty",
   "seventy",   
   "eighty",
   "ninety"
};

int main(int argc, char **argv)
{ 
    numbers(argc, argv);
    return 0;
}
/** 
 * numbers - starts the program
 */
void numbers(int argc, char **argv)
{
    FILE *file_read;
    FILE *file_write;
    
    if(argc == 1) /*read file*/ 
    {
        file_read = stdin;
        file_write = stdout;
        HandleInputOutput(file_read,file_write);
    }
    if(argc == 2) /*read file*/ 
    {
        file_read =fopen(argv[1],"r");
        file_write = stdout;
        HandleInputOutput(file_read,file_write);
        fclose(file_read);
    }
    if(argc == 3)
    {
        file_read = fopen(argv[1],"r");
        file_write =fopen(argv[2],"w");
        HandleInputOutput(file_read,file_write);
        fclose(file_read);
        fclose(file_write);

    }

}


/**
 * HandleInputOutput - gets numbers from file
 * @file_read - read source
 * @file_write - write dest
 */
void HandleInputOutput(FILE * file_read,FILE *file_write)
{
    char line[100];
    if(file_read == NULL) 
    {
        printf("Erorr opening read file");
        assert(1);
    }   
    if(file_read == NULL)
    {
        printf("Eroor opening write file");
        assert(1);
    }

      
    while(fgets(line,sizeof(line),file_read) != NULL)
    {
        printNumbers(line,file_write);
    }
    
    
}



/**
 * printNumbers - prints a line of numbers as words.
 * line - string of numbers from 1-99
 * output - file to write to
 */
void printNumbers(char * line,FILE * output)
{
    char * next_char = line;
    char number_word[20];
    unsigned int num;
    strcat(line, " ");

    while(*next_char != '\0')
    {

        if(isdigit(*next_char)) /*read while space or tab or end of line*/
        {
            num = *next_char - '0';
            if(isdigit(*(next_char+1)))
            {
                num = num*10 + (*(++next_char)-'0');
    
            }

            numberToWord(num,number_word); /*number to word*/ 
            fprintf(output,"%s \n",number_word);
            next_char++;
        }
        else
        {
            next_char++;
        }
    }
}
/**
 * numberToWord - Convert an Integer into a string using @number_names
 * @num         - Sourse number
 * @word        - Output number as a string
 */
void numberToWord(int num,char * word)
{
    /*Word equals zero only when its the only digit in the number*/
    if(num == 0) 
    {
        snprintf(word,MAX_WORD_LENGTH,"%s",number_names[0]); /*snprintf is a safe*/
    }
    else
    {
        /*If the number between 1-20 the number values corresponds to the correct index in number_names */
        if(num<20) 
        {
            snprintf(word,MAX_WORD_LENGTH,"%s",number_names[num]);
        }
        else /*If the number is between is greater then 20*/
        {
            if(num%10 != 0)
            {
                snprintf(word,MAX_WORD_LENGTH,"%s %s",number_names[18 + num/10], number_names[num % 10]);
                printf("A: %ld\n",sizeof(word));
            }else
            {
                snprintf(word,MAX_WORD_LENGTH,"%s",number_names[18 + num/10]);

            }
        }
    }
    
    
}