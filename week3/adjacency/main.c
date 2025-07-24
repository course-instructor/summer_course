#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "header/adjacency.h"
#define MAX_INPUT 100

/*false*/
int main(void)
{
    adjmat tree;
    int v,u;
    print_instructions();
    get_metrix_form_input(&tree);
    printf("\n");
    adjmat_print(tree,N,N);
    if(adjmat_validation(tree))
    {
        scanf("%*c"); 

        while(1)
        {
        printf("\n");
        get_two_numbers(&v,&u);
        if(path(tree,v,u) == TRUE)
        {
            printf("path exists!\n");
            
        }
        else
        {
            printf("path dont exists!\n");
        }

        }
        
    }
    else
    {
        get_metrix_form_input(&tree);
        printf("\n");
        adjmat_print(tree,N,N);

    }


    return 0;
}
/**
 * @brief prints welcome screen for user
 */
void print_instructions(void)
{
    printf("\n--- Tree Path Finder ---\n");
    printf("In this funny program you enter an adjmat metrix, and check if there an existing path form point a to point b!\n");
    printf("Max amont of nodes that are allowed is { %d } !\n", N);
}


/**
 * @brief gets input from user of two numbers between 1 and N
 * @param v source node
 * @param u dest node
 */
int get_two_numbers(int *v, int *u) {
    char input[MAX_INPUT];
    char *token1;
    char *token2;
    char *extra;
    char *end1;
    char *end2;
    long val1;
    long val2;
    int valid = 0;

    while (!valid) {
        printf("Enter two numbers (1-%d) or type \"exit\": ", N);

        if (fgets(input, sizeof(input), stdin) == NULL) 
        {
            fprintf(stderr, "Input error\n");
            break;
        }

        /* remove trailing newline */
        input[strcspn(input, "\n")] = '\0';

        /* check for exit command */
        if (strcmp(input, "exit") == 0) 
        {
            printf("Exiting...\n");
            exit(0);
        }

        token1 = strtok(input, " ");
        token2 = strtok(NULL, " ");
        extra  = strtok(NULL, " ");

        if (token1 != NULL && token2 != NULL && extra == NULL) {
            val1 = strtol(token1, &end1, 10);
            val2 = strtol(token2, &end2, 10);

            if (*end1 == '\0' && *end2 == '\0' &&
                val1 > 0 && val1 <= N &&
                val2 > 0 && val2 <= N &&
                val1 != val2) 
            {
                *v = (int)val1 - 1;
                *u = (int)val2 - 1;
                valid = 1;
            } 
            else 
            {
                printf("Invalid numbers. Must be integers between 1 and %d, and not be equal\n", N);
            }
        } 
        else 
        {
            printf("Please enter exactly two numbers or type \"exit\"\n");
        }
    }

    return valid;
}




/**
 * @brief get metrix from user input, user may enter any charcters , but only 1 or 0 would be taken into account
 * @param metrix pointer adjmat metrix
 */
void get_metrix_form_input(adjmat * metrix)
{
    char input;
    printf("input 1 or 0:\n");
    for (int i = 0; i < N; i++)
    {
        printf("\n[line %d]\n",i+1);
        for (int j = 0; j < N; j++)
        {
            input = fgetc(stdin);
            while(input != '0' && input != '1')
            {
               
                input = fgetc(stdin);
            }
            (*metrix)[i][j] = input - '0';   
            input = 0;  
        }
    }

}
/**
 * @brief checks if adjmat metrix is legal - if it has only one head and no more then one node pointing to another node
 * @param metrix adjmat metrix
 */
bool adjmat_validation(adjmat matrix)
{
    bool is_valid = TRUE;
    int col,row;
    int zero_cols = 0;

    bool is_col_zero = TRUE;
    int num_of_ones = 0;

    /*Check if there are at least a single 1 in a column */
    for (col = 0; col < N; col++)
    {
        for (row = 0; row < N; row++)
        {
            if (matrix[row][col] == 1)
            {
                is_col_zero = FALSE;
                num_of_ones++;

            }
            
        }
        if(is_col_zero == TRUE)
        {
            zero_cols++;
        }
        if(num_of_ones > 1)
        {
            is_valid = FALSE;
        
        }
        num_of_ones = 0;
        is_col_zero = TRUE;
       
    }
    


    return zero_cols == 1 && is_valid;

}
/**
 * @brief prints metrix
 * @param row amont rows to print
 * @param cols amont of columns to print
 */
void adjmat_print(adjmat matrix, int rows, int cols) 
{
    int i,j;
    printf("Matrix (%d x %d):\n", rows, cols);
    printf(" ");
    for (i = 1; i <= cols; i++)
    {
        printf(" %4d", i);  

    }

    printf("\n");

    for (i = 1; i <= rows; i++) {
        printf("%i |",i);
        for (j = 0; j < cols; j++) {
            printf(" %4d", matrix[i-1][j]);  
        }
        printf(" |\n");
    }
}
