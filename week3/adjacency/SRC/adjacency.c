#include <stdio.h>
#include "../HEADER/adjacency.h"
#include <stdlib.h>


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
    if(!(u<0 || u >= N || v < 0 || v >= N)) 
    {
        retval = FALSE;
    }
    else
    {
        retval = find_path(adjmat, u, v);
    }
    return retval;
}

