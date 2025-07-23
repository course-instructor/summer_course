#include "./HEADER/adjacency.h"
#include <stdlib.h>  
#include <stdio.h>

/**
 * @brief Main function to demonstrate the adjacency matrix path finding.
 * 
 * @return int Exit status of the program.
 */
int main(void)
{
    adjmat my_adjmat;
    int retval;
    int is_eof_u, is_eof_v;
    init_matrix(&my_adjmat);
    retval = print_matrix(&my_adjmat);
    /* if the matrix has valid values */
    if(retval)
    {
        int u, v;
        printf("enter the value of u: \n");
        is_eof_u = scanf("%d", &u);
        retval = check_eof(is_eof_u);
        /* if we didnt get an EOF in u we continue*/
        if(retval)
        {
            printf("enter the value of v: \n");
            is_eof_v = scanf("%d", &v);
            retval = check_eof(is_eof_v);
            if(!retval)
            {
                printf("EOF reached for v input.\n");
                retval = 0;
            }
            /* if we didnt get an EOF for v we continue*/
            if(retval)
            {
                /* same logic but in an endless loop until we get -1 -1 or ctrl + d*/
                while(1)
                {
                    if(!(u < 0 || u >= N || v < 0 || v >= N))
                    {
                        retval = path(&my_adjmat, u, v);
                        if(retval)
                        {
                            printf("There IS a path from %d to %d.\n", u, v);
                        }
                        else
                        {
                            printf("There is NO path from %d to %d.\n", u, v);
                        }
                        printf("Enter the value of u: \n");
                        is_eof_u = scanf("%d", &u);
                        retval = check_eof(is_eof_u);
                        if(retval)
                        {
                            printf("Enter the value of v: \n");
                            is_eof_v = scanf("%d", &v);
                            retval = check_eof(is_eof_v);
                        }
                        else
                        {
                            printf("EOF reached for u input.\n");
                            break;
                        }
                        if(!retval)
                        {
                            printf("EOF reached for v input.\n");
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    printf("Exiting the program.\n");
    return 0;
}