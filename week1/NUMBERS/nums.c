#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const char* numbs[] =
{
    "zero", "one", "two", "three", "four",
    "five", "six", "seven", "eight", "nine",
    "ten", "eleven", "twelve", "thirteen", "fourteen",
    "fifteen",
    "twenty", "thirty", "forty", "fifty"
};



char * exchange(int num)
{
    /**
     * the function converts a number from 0 to 99 to its string representation
     * 
     * var num: the number to convert
     * var type: int
     * 
     * returns: a pointer to a string that contains the number in words
     * rtype: char * (string)
    */
    char * ret ;

    char buffer[50];
    buffer[0] = '\0'; // Initialize buffer 



    if(num < 16 )   
    {
        snprintf(buffer, sizeof(buffer), "%s", numbs[num]);
    }

    else if (num < 20)
    {
        if(num == 18)
        {
            snprintf(buffer, sizeof(buffer), "%s", "eighteen");
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%s%s", numbs[num - 10], "teen");
        }
    }

    else
    {

        if(num / 10 < 6)
        {
            snprintf(buffer, sizeof(buffer), "%s", numbs[num / 10 + 14]);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%s%s", numbs[num / 10], "ty");
        }

        if(num % 10 != 0)
        {
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), " %s", numbs[num % 10]);

        }
    }

    ret = malloc(strlen(buffer) + 2); // Allocate memory for the buffer and add space for the new line char and null at the end
    sprintf(ret, "%s\n", buffer); // copy the buffer to ret and add new line

    return (ret);
}


void error(const char *msg)
{
    /**
     * the function prints an error message to stderr
     * 
     * var msg: the error message to print
     * var type: const char *
     * 
     * returns: void
    */
    fprintf(stderr, "Error: %s\n", msg);
}

void file_handle(FILE * inp_file, FILE * out_file)
{
    /**
     * the function handles the input file and output file (even if the output file is NULL or input is stdin)
     * it reads from the file char by char and converts every number and writes it to the correct output
     * 
     * var inp_file: the input file to read from (can be stdin)
     * var out_file: the output file to write to (can be NULL)
     * 
     * type inp_file: FILE *
     * type out_file: FILE *
     * 
     * returns: void
     */

    char last_char = ' '; // Variable to hold the last character read from the file

    char * out_str = NULL; // Pointer to hold the converted number string

    int number_to_convert = 0; // stores the current number that gets converted

    int index = 0; // index for the current char

    

    while ((last_char != EOF) && (last_char != '\n' || inp_file != stdin)) //check if the input has ended
    {
        if ((last_char == '\t' || last_char == ' ' || last_char == '\n') && index > 0)/// check if the last char is blank spaces and there was a nubmber 
        {

                out_str = exchange(number_to_convert); // Convert the number to words
                
                if(out_file) // Check if an output file is provided
                {
                    fwrite(out_str, sizeof(char), strlen(out_str), out_file); // Write the converted number to the output file
                }
                else // if output file isnt provided output to stdout
                {
                    printf("%s", out_str); // Print the converted number to stdout
                }

                free(out_str); // Free the allocated memory for the converted number
                number_to_convert = 0; // Reset the number for the next iteration
                index = 0; // Reset the index for the next number
        }

        else
        {
            if(( last_char >= '0') && (last_char <= '9'))
            {
                number_to_convert *= 10; // Shift the current number left
                number_to_convert += last_char - '0'; // Convert the current char to int and add it to the number
                index++; // Move to the next char in the buffer
            }
        }

        last_char = fgetc(inp_file); // get the next last char
    }

    if (index > 0) // Check if there is a number left to convert that wasnt conveted because of the EOF that ended the loop
    {
        out_str = exchange(number_to_convert); // Convert the last number to words
        
        if(out_file) // Check if an output file is provided
        {
            fwrite(out_str, sizeof(char), strlen(out_str), out_file); // Write the last converted number to the output file
        }
        else // if output file isnt provided output to stdout
        {
            printf("%s", out_str); // Print the last converted number to stdout
        }
        free(out_str); // Free the allocated memory for the last converted number
    }
    
}


void numbers(int argc, char *argv[])
{
    /**
     *  the function handles from where the input comes and where the output goes based on the arguments:
     *  if there are no arguments it reads from stdin and outputs to stdout
     *  if there is one argument it reads from the file and outputs to stdout
     *  if there are two arguments it reads from the first file and outputs to the second file
     * 
     *  var argc: the number of arguments passed to the program
     *  var argv: the arguments passed to the program
     * 
     *  type argc: int
     *  type argv: char ** (array of strings)
     * 
     *  returns: void
     */


    FILE * inp_file = NULL; // Pointer for input file
    FILE * out_file = NULL; // Pointer for output file

    switch(argc)
    {
        case 1: // No arguments provided, read from stdin output to stdout
            printf("please enter numbers to convert that are seperated by blank spaces from 0 to 99\n");
            file_handle(stdin, NULL); // Handle stdin input
            break;
        
        case 3: //two args provided read first open output file then read input file with case 2
            out_file = fopen(argv[2], "w");
            if (out_file == NULL) //if the file didnt open
            {
                error("opening output file.");
            }


        case 2: // One argument provided, read from the file, write to stdout
            inp_file = fopen(argv[1], "r");
            if (inp_file == NULL) //if the file didnt open
            {
                error("opening input file."); 
            }
            file_handle(inp_file, out_file); // Handle the input file and output file (if no output it will be NULL)
            fclose(inp_file); // after handeling the input file close it
            break;        
    }

    if(out_file && argc == 3) // if there is an output file provided
    {
        fclose(out_file); //close the output file
    }


}
int main(int argc, char *argv[])
{
    numbers(argc, argv);
    return 0;
}