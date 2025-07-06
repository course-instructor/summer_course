#include <stdio.h>
#include <ctype.h>

#define G_ARR_LENGTH 80

void contract(char input_string[], char output_string[])
{
    /**
     * @brief the function  gets 2 strings it reads from the first and rewrites it to the second after reformating
     * @param input_string the first string that is being formatted. (read not changed)
     * @param output_string the second string that will contain the result of the formatting.
     */
    
    int output_string_index = 0; 
    int input_string_index = 0; // index of the current char that is being red from input_string
    int sequence_end_index; // index of the end of the current sequence of following chars with ascending ascii values
   
    for(input_string_index = 0; (input_string_index < G_ARR_LENGTH) && (input_string[input_string_index] != '\0'); input_string_index++)
    {
        for(sequence_end_index = input_string_index; (sequence_end_index + 1 < G_ARR_LENGTH) && ( input_string[sequence_end_index] + 1 ==   input_string[sequence_end_index + 1]) && (isdigit(input_string[sequence_end_index + 1]) || isalpha(input_string[sequence_end_index + 1])); sequence_end_index++)
        {
            // do nothing, just find the end of the sequence
        }
        
        if(sequence_end_index - input_string_index > 1) // the length of the sequence is more than 3
        {
            output_string[output_string_index++] = input_string[input_string_index]; //first char in the sequence
            output_string[output_string_index++] = '-';
            output_string[output_string_index++] = input_string[sequence_end_index]; // last char in the sequence
            input_string_index = sequence_end_index;
        }
        else
        {
            output_string[output_string_index++] = input_string[input_string_index];
        }
        input_string_index++;
    }

}

int main()
{
    /**
     * @brief this program gets a line of input from the user and reformats it using the contract function.
     */
    
    char input_string [G_ARR_LENGTH]; 
    char output_string [G_ARR_LENGTH];


    printf("please enter a line of input (up to 80 chars)\n");
    fgets(input_string,G_ARR_LENGTH, stdin); // get user input
    printf("your input:\n");
    printf("%s", input_string);
    contract(input_string, output_string); 
    printf("\nafter reformating:\n");
    printf("%s", output_string);
    return 0;
}
