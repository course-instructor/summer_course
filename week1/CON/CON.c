#include <stdio.h>
#include <string.h>

#define ASCII_DIFF 1
#define MAX_LEN 80

/*
 *  Compresses sequences of consecutive characters (like abc) to a short form (a-c).
 *
 *  input The original string.
 *  updating the requested array
 */
int contract(const char input[], char output[])
{
    int retval = 0;
    int read_idx = 0;
    int write_idx = 0;

    /* check if input is empty */
    if (input[0] == '\0')
    {
        retval = 1; /* error code */
    }
    else
    {
        while (input[read_idx] != '\0') /* while we are not in the end */
        {
            if(input[read_idx + 1]  != '\0' && input[read_idx+2] != '\0')
            {
                if (input[read_idx + 1] - input[read_idx] == ASCII_DIFF &&
                    input[read_idx + 2] - input[read_idx + 1] == ASCII_DIFF) /* if there is a sequence of 3 values */
                {
                    output[write_idx++] = input[read_idx++];
                    while (input[read_idx + 1] - input[read_idx] == ASCII_DIFF)
                    {
                        read_idx++; /* moving until the sequence is over */
                    }
                        
                    output[write_idx++] = '-';
                    output[write_idx++] = input[read_idx++]; /* copying the final char of the sequence */
                    continue;
                }
            }
            
            output[write_idx++] = input[read_idx++]; /* copying any other char that is not in a sequence */
        }
        output[write_idx] = '\0';
    }

    return retval;
}

/*
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
    int retval = 0;

    printf("Please enter your sequence:\n");
    if (fgets(input, MAX_LEN, stdin) == NULL)
    {
        retval = 1;
    }
    else
    {
        retval = contract(input, shorted);

        if (retval == 0)
        {
            printf("The original input was:\n%s\n", input);
            printf("The shorted string is:\n%s\n", shorted);
        }
        else
        {
            printf("Error processing the input.\n");
        }
    }

    return retval;
}
