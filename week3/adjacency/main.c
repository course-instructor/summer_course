#include "./HEADER/adjacency.h"
#include <stdlib.h>  
#include <stdio.h>

/**
 * @brief we get values to the matrix of adjacency.
 * 
 * @param adjmat the matrix we get values into
 * @return int 
 */

int init_matrix(adjmat_ptr adjmat)
{
    int row, col;
    printf("The size of N is: %d\n", N);
    for(row = 0; row < N; row++)
    {
        for(col = 0; col < N; col++)
        {
            printf("Enter the value for adjency matrix[%d][%d]: ", row, col);
            scanf("%d", &adjmat->matrix[row][col]);
        }
    }
    return 1;
}

/**
 * @brief this function prints the matrix of adjacency.
 * 
 * @param adjmat the matrix we print
 * @return int 
 */

int print_matrix(adjmat_ptr adjmat)
{
    int row, col;
    for(row = 0; row < N; row++)
    {
        for(col = 0; col < N; col++)
        {
            printf("%d ", adjmat->matrix[row][col]);
        }
        printf("\n");
    }
    return 1;
}

/**
 * @brief Main function to demonstrate the adjacency matrix path finding.
 * 
 * @return int Exit status of the program.
 */
int main(void)
{
    adjmat my_adjmat;
    int is_eof_u, is_eof_v;
    init_matrix(&my_adjmat);
    print_matrix(&my_adjmat);
    int u, v;
    printf("enter the value of u: \n");
    is_eof_u = scanf("%d", &u);
    if(is_eof_u != EOF)
    {
        printf("enter the value of v: \n");
        is_eof_v = scanf("%d", &v);
        if(is_eof_v != EOF)
        {
           while(u != -1 && v != -1)
           {
            int result = path(&my_adjmat, u, v);
            printf("Is there a path from %d to %d? %s\n", u, v, result ? "Yes, Good job!" : "No :(");
            printf("enter the value of u: \n");
            scanf("%d", &u);
            printf("enter the value of v: \n");
            scanf("%d", &v);
           }
        }
    }
    printf("Exiting the program.\n");
    return 0;
}