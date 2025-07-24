#include <stdio.h>
#include "../HEADER/adjacency.h"
#include <stdlib.h>


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
 * @brief we check if the input is correct
 * 
 * @param adjmat a poi nter the matrix we check
 * @return int retval of 0 if wrong and 1 if correct
 */

int check_init_mat(adjmat_ptr adjmat)
{
    int retval = 1;
    int row, col;
    int count_ones;
    int count_zero_coulmn = 0;
    for(col = 0; col < N; col++)
    {
        if(retval == 1)
        {
            count_ones = 0;
            for(row = 0; row < N; row++)
            {
                if(adjmat->matrix[row][col] != 0 && adjmat->matrix[row][col] != 1 )
                {
                    retval = 0;
                    printf("The matrix can only get 0's and 1's\n");
                }
                else
                {
                    if(adjmat->matrix[row][col] == 1)
                    {
                        if(row == col)
                        {
                            retval = 0; 
                            printf("A node can't point on itself\n");
                            break;
                        }
                        if(count_ones == 1)
                        {
                            retval = 0;
                            printf("there are two paths to the same node, the node is: %d\n",col);
                            break;
                        }
                        else
                        {
                            count_ones++;
                        }
                    }
                }
            }
            if(count_ones == 0)
            {
                if(count_zero_coulmn > 0)
                {
                    retval = 0;
                    printf("There cannot be two columns that will be as a father\n");
                }
                else
                {
                    count_zero_coulmn++;
                }
            }
        }
        
    }
    return retval;
}

/**
 * @brief this function prints the matrix of adjacency.
 * 
 * @param adjmat the matrix we print
 * @return int 
 */

int print_matrix(adjmat_ptr adjmat)
{
    int retval = check_init_mat(adjmat);
    if(retval)
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
    }
    
    return retval;
}


/**
 * @brief this is a recursive function that checks if there is a path from u to v.
 * 
 * @param adjmat our matrix of adjacency
 * @param u the current father we check
 * @param v the value we want to find a route to
 * @return int true or false
 */

int find_path(adjmat_ptr adjmat, int u, int v)
{
    
    int row, col;
    int count_zeros = 0;
    for(col  = 0; col < N; col++)
    {
        /*if we got to the destiniton*/
        if(adjmat->matrix[u][v] == 1)
        {
            return TRUE;
        }
        /* we want to move onto the next child, and check if it's a leaf*/
        if(adjmat->matrix[u][col] == 0)
        {
            count_zeros++;
        }
        else
        {
            u = col;
            return find_path(adjmat, u, v);
        }
    }
    if(count_zeros == N)
    {
        return;
    }
    
}

/**
 * @brief the function checks if there is a u is father of v.
 * 
 * @param adjmat pointer to the matrix 
 * @param u the "maybe" father of v
 * @param v the "maybe" son of u
 * @return int true or false
 */


int path(adjmat_ptr adjmat,int u , int v)
{
    int retval;
    if(u<0 || u >= N || v < 0 || v >= N) 
    {
        retval = FALSE;
    }
    else
    {
        retval = find_path(adjmat, u, v);
    }
    return retval;
}
/**
 * @brief This function checks if the input is EOF.
 * 
 * @param flag_u the flag we check
 * @return int 1 if not EOF, 0 if EOF
 */
int check_eof(int flag_u)
{
    int retval;
    if(flag_u == EOF)
    {
        retval = 0;
    }
    else
    {
        retval = 1;
    }
    return retval;
}

