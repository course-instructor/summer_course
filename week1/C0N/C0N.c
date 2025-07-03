#include <stdio.h>
#include <ctype.h>

#define ARR_LENGTH 80

void contract(char s1[], char s2[])
{
    /**
     * the function gets 2 strings and formats the first string so that every 
     * sequence of following letters or numbers longer than 2 is reformated into the first char "-" the last char of the sequence.
     * and then stores the result in the second string.
     * 
     * var s1: the first string that is being formatted. (read not changed)
     * var s2: the second string that will contain the result of the formatting.
     * 
     * s1 type: char[] (string)
     * s2 type: char[] (string)
     * 
     * return: writes the frmated first string into the second.
     * rtype: void
    */
    int s2_index = 0; 
    int i = 0; // index of the current char that is being red from s1
    int j; // index of the end of the current sequence of following chars with ascending ascii values
   
    while ( (i < ARR_LENGTH) && (s1[i] != '\0'))
    {
        j = i;
        while((j + 1 < ARR_LENGTH) && ( s1[j] + 1 ==   s1[j + 1]) && (isdigit(s1[j + 1]) || isalpha(s1[j + 1])))
        {
            j++;
        }
        
        if(j - i > 1) // the length of the sequence is more than 3
        {
            s2[s2_index++] = s1[i]; //first char in the sequence
            s2[s2_index++] = '-';
            s2[s2_index++] = s1[j]; // last char in the sequence
            i = j;
        }
        else
        {
            s2[s2_index++] = s1[i];
        }
        i++;
    }


}

int main()
{
    /**
    get a line of input from the user using fgets
    call a reformating function on the input
    print the before and afater    
    */
    char s1 [ARR_LENGTH]; 
    char s2 [ARR_LENGTH];


    printf("please enter a line of input (up to 80 chars)\n");
    fgets(s1,ARR_LENGTH, stdin); // get user input
    printf("your input:\n");
    printf("%s", s1);
    contract(s1, s2); 
    printf("\nafter reformating:\n");
    printf("%s", s2);
    return 0;
}
