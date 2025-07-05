#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define NUM_OF_UNIQUE_NUMBER_WORDS 28
/*Array of each uniqe name for a number in english from 0 - 99*/
static const char *number_names[NUM_OF_UNIQUE_NUMBER_WORDS] = {
   "one",
   "zero",
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
void numberToWord(int num,char * word);
void readFromInputFile(char * path);
void printNumbers(char * line);
void readFromUser(void);
void numbers(int argc, char **argv);
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
    if(argc > 1) /*read file*/ 
    {
        readFromInputFile(argv[1]);
    }else{
        readFromUser();
    }


}


/**
 * readFromInputFile - gets numbers from file
 * @path - file path
 * 
 */
void readFromInputFile(char * path)
{
    
    char line[100];

    FILE *fptr = (FILE*)fopen(path, "r");
        if(fptr== NULL){
            printf("Could not find the file!\n");
        }
        else
        {
            while(fgets(line,sizeof(line),fptr) != NULL)
            {
                printNumbers(line);
            }
            fclose(fptr);
        }

}
/**
 * readFromUser - ask user input from command line
 */
void readFromUser(void)
{
    char line[100];
    fgets(line,sizeof(line),stdin);
    printNumbers(line);  
}

/**
 * printNumbers - prints a line of numbers as words.
 * @line - string of numbers from 1-99
 */
void printNumbers(char * line)
{
    char *start = line;
    char * next = start;
    char number_word[30];
    strcat(line, " ");

    while(*next != '\0')
    {
        if(*next == ' ' || *next == '\t' || *next =='\0') /*read while space or tab or end of line*/
        {
            *next = '\0';
            numberToWord(atoi(start),number_word); /*number to word*/ 
            printf("%s \n",number_word); 
            start = ++next; /* go to the next number*/

        }
        else
        {
            next++;
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
        strcpy(word,number_names[num]);
    }
    else
    {
        /*If the number between 1-20 the number values corresponds to the correct index in number_names */
        if(num<20) 
        {
            strcpy(word,number_names[num]);
        }
        else /*If the number is between is greater then 20*/
        {
            strcpy(word,number_names[18 + num/10]); /*The index of the first word (tens) equals to value of the digit +18(for skipping the numbers between 1-20)*/
            strcat(word , " "); /*Space between the words*/
            if(num%10 != 0)
            {
                strcat(word, number_names[num % 10]); /*Get the world for the remining digit*/
            }
        }
    }
    
    
}