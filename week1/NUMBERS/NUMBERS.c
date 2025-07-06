#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 50

/* we initialize a "dict" so we can find the word that matches for the number we got. */
enum num_word {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN,
    TWELVE, THIRTEEN, FOURTEEN, FIFTEEN, SIXTEEN, SEVENTEEN, EIGHTEEN, NINETEEN,
    TWENTY, THIRTY, FOURTY, FIFTY, SIXTY, SEVENTY, EIGHTY, NINTY
};

const char *num_words[] =
{
    "zero", "one", "two", "three", "four", "five",
    "six", "seven", "eight", "nine", "ten",
    "eleven", "twelve", "thirteen", "fourteen", "fifteen",
    "sixteen", "seventeen", "eighteen", "nineteen",
    "twenty", "thirty", "forty", "fifty", "sixty",
    "seventy", "eighty", "ninety"
};

/*
 @brief the function gets the input file and converts it to an array of int's.
 @param input_file the file from which we read the sequence of numbers
 @param size pointer to int that will store how many numbers we read
 @return it returns an array of ints.
*/
int* seperate_nums(FILE *input_file, int *size)
{
    int capacity = 10;
    int *numbers = malloc(capacity * sizeof(int));
    if (numbers == NULL)
    {
        fprintf(stderr, "realloc failed\n");
        exit(EXIT_FAILURE);
    }

    int number;
    *size = 0;

    /* read numbers until EOF */
    while (fscanf(input_file, "%d", &number) == 1)
    {
        if (*size == capacity)
        {
            capacity *= 2;
            numbers = realloc(numbers, capacity * sizeof(int));
            if (numbers == NULL)
            {
                fprintf(stderr, "realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
        numbers[(*size)++] = number;
    }

    return numbers;
}

/*
 @brief this is the function that prints the numbers in words
 @param input_file the file that we got our input from
 @param output_file the file that we will write our output to
*/
void print_words_num(FILE *input_file, FILE *output_file)
{
    int nums_size;
    int *nums = seperate_nums(input_file, &nums_size);
    int nums_idx;

    for (nums_idx = 0; nums_idx < nums_size; nums_idx++)
    {
        int number = nums[nums_idx];

        if (number <= 20)
        {
            /* if num is below or equal 20 we can just write it from the array we created */
            fprintf(output_file, "%s\n", num_words[number]);
        }
        else
        {
            int tens = number / 10;
            int units = number % 10;

            char tens_in_words[100];
            /* if the number is greater than 20 we will save the word that represents the tens */
            strcpy(tens_in_words, num_words[TWENTY + tens - 2]);

            if (units != 0)
            {
                strcat(tens_in_words, " ");
                /* we add to the word that represents the tens the word that represents the units */
                strcat(tens_in_words, num_words[units]);
            }
            fprintf(output_file, "%s\n", tens_in_words);
        }
    }
    free(nums);
}

/*
 @brief this is the function that activates our main correctly
 @param argc this is the first argument that will represent how many args we got
 @param argv this is a string that contains all of the args we got
 @return we return an int that represents if the program run correctly and successfully
*/
int run_program(int argc, char *argv[])
{
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    /* in case we got only one argument */
    if (argc == 1)
    {
        input_file = stdin;
        output_file = stdout;
    }
    /* in case we got only two arguments */
    else if (argc == 2)
    {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL)
        {
            fprintf(stderr, "cant open input file %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        output_file = stdout;
    }
    /* in case we got three arguments */
    else if (argc == 3)
    {
        /* we will open the first file for reading */
        input_file = fopen(argv[1], "r");
        if (input_file == NULL)
        {
            fprintf(stderr, "cant open input file %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        /* we will open the second file for writing */
        output_file = fopen(argv[2], "w");
        if (output_file == NULL)
        {
            fprintf(stderr, "cant open output file %s\n", argv[2]);
            fclose(input_file);
            return EXIT_FAILURE;
        }
    }
    /* if we have more than 3 args */
    else
    {
        /* it cant be open so we terminate it */
        fprintf(stderr, "too many arguments\n");
        return EXIT_FAILURE;
    }

    print_words_num(input_file, output_file);

    /* here we close all the files */
    if (input_file != stdin)
    {
        fclose(input_file);
    }
    if (output_file != stdout)
    {
        fclose(output_file);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    run_program(argc, argv);
}
