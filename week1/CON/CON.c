#include <stdio.h>
#include <string.h>

#define ASCII_DIFF 1
#define MAX_LEN 80

/**
 *  Compresses sequences of consecutive characters (like abc) to a short form (a-c).
 *
 *  input The original string.
 *  updating the requested array
 */
void contract(const char input[], char output[])
{
    int input_index = 0;
    int output_index = 0;

    while (input[input_index] != '\0')// while we are not in the end
    {
        if (input[input_index + 1] - input[input_index] == ASCII_DIFF &&
            input[input_index + 2] - input[input_index + 1] == ASCII_DIFF)//if there is a sequence of 3 values
        {
            output[output_index++] = input[input_index++];
            while (input[input_index + 1] - input[input_index] == ASCII_DIFF)
                input_index++;//moving until the sequence is over
            output[output_index++] = '-';
            output[output_index++] = input[input_index++];//copying the final char of the sequence
        }
        else
        {
            output[output_index++] = input[input_index++];//copying any other char that is not in a sequence
        }
    }
    output[output_index] = '\0';
}
/**
 *  input The original string.
 *  printing the shorted string
 * 
 *  input = the input string
 *  shorted = the shorted array of the string
 * 
 *  input and shorted are arrays of chars
 */
int main()
{
    char input[MAX_LEN];
    char shorted[MAX_LEN];

    printf("Please enter your sequence:\n");
    fgets(input, MAX_LEN, stdin);

    contract(input, shorted);

    printf("The original input was:\n%s\n", input);
    printf("The shorted string is:\n%s\n", shorted);

    return 0;
}