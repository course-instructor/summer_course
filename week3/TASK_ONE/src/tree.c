#include <stdio.h>
#include <ctype.h>
/* isspace */

#include "../headers/tree.h"

/**
 * @brief the function checks recursivly if there is a path between two nodes in a tree in the given direction, by going 'backwords' from bottom to top.
 * 
 * @param mat the adjacency matrix of the tree
 * @param u the starting node
 * @param v tne destination node
 * @return boolean_e is there a path from u to v?
 */
boolean_e path(adjmat mat, int u, int v)
{
    boolean_e ret ;
    int i;

    if(u == v)
    {
        ret = TRUE;
    }
    else if(u < 0 || v < 0 || u >= N || v >= N )
    {
        ret = FALSE;
    }
    else
    {
        ret = FALSE;

        for( i = 0; i < N && ret == FALSE; i++)
        {
            if(mat[i][v] == TRUE)
            {
                ret =  path(mat, u, i);
            }
        }
    }

    return ret;
}

/**
 * @brief counts the amount of nodes that connect to the node u directly
 * 
 * @param mat the adjacency matrix of the tree
 * @param u the node to count the fathers of
 * @return int the number of fathers that u has
 */
int count_fathers(adjmat mat, int u)
{
    int count = 0;
    int i;

    for(i = 0; i < N; i ++)
    {
        if(mat[i][u] == TRUE)
        {
            count++;
        }
    }

    return count;
}

/**
 * @brief the function counts the number of direct paths that go from node u
 * 
 * @param mat the adjacency matrix of the tree
 * @param u the node to count the children of
 * @return int the number of children that u has
 */
int count_children(adjmat mat, int u)
{
    int count = 0;
    int i;
    for(i = 0; i < N; i++)
    {
        if(mat[u][i] == TRUE)
        {
            count++;
        }
    }

    return count;
}

/**
 * @brief the function checks if the input is a valid tree.
 * 
 * @param mat the adjacency matrix of the tree
 * @return boolean_e  whether the input is a valid tree or not.
 */
boolean_e check_input(adjmat mat)

{
    int count_roots = 0;
    int fathers;

    boolean_e ret = TRUE;
    int u;

    for (u = 0; (u < N) && (ret == TRUE) && (count_roots < 2); u++)
    {
        fathers = count_fathers(mat, u);
        if(fathers > 1)
        {
            ret = FALSE;
        }

        else if(fathers == 0)
        {
            if(count_children(mat, u) > 0)
            {
                count_roots++;
            }
        }
    }
    if(count_roots != 1)
    {
        ret = FALSE;
    }

    return ret;
}

/**
 * @brief Get the adjacency matrix from the user.
 * 
 * @param mat the adjacency matrix to fill with user input
 */
void get_mat(adjmat  mat)
{
    char ch;
    int i, j;

    printf("enter adjmat values (0 or 1) for %d nodes:\n", N);

    for(i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            printf("mat[%d][%d]:\t", i, j);
            fflush(stdout);

            do
            {
                ch = fgetc(stdin);
            }while(isspace(ch));
            if(ch == '0')
            {
                mat[i][j] = FALSE;
            }
            else if(ch == '1')
            {
                mat[i][j] = TRUE;
            }
            else
            {
                printf("Invalid input. enter 0 or 1.\n");
                j--; /* repeat this input */
            }
        }
    }

}

/**
 * @brief Get a number from the user, which should be a non-negative integer, or -1 to exit.
 * 
 * @return int 
 */
int get_num(void)
{
    boolean_e still_reading = TRUE;
    int num = -1; /* default value */
    char ch;


    while(still_reading)
    {
        ch = fgetc(stdin);
        if(ch == '-')
        {
            ch = fgetc(stdin);
            if(ch == '1' && isspace(fgetc(stdin)) && num == -1) /* exactly -1 was entered */
            {
                still_reading = FALSE;
            }

            else
            {
                printf("invalid input, try again\n");
                num = -1; /* reset num to default */
            }
        }
        else if(ch <= '9' && ch >= '0')
        {
            if(num == -1)
            {
                num = 0;
            }
            num = num * 10 + (ch -'0');
        }

        else if(isspace(ch))
        {
            if(num != -1) /* space after an input... */
            {
                still_reading = FALSE;
            }
        }

        else
        {
            printf("invalid input, try again\n");
            num = -1; /* reset num to default */
        }
    }
    return num;
}

/**
 * @brief the program gets an adjacency matrix from the user, checks if it is a valid tree, and then checks if there is a path between two nodes from the user until it ends with -1 -1.
 * 
 * @return int 
 */
int main()
{

    adjmat mat;
    boolean_e done;
    int u,v;

    get_mat(mat);

    while(check_input(mat) == FALSE)
    {
        printf("Invalid input.not a tree\n");
        get_mat(mat);
    }


    done = FALSE;

    while(done == FALSE)
    {
        printf("enter two nodes to check if they are connected:\n");
        u = get_num();
        v = get_num();

        if(u == -1 && v == -1)
        {
            printf("exiting\n");
            done = TRUE;
        }

        else if(u < 0 || v < 0 || u >= N || v >= N)
        {
            printf("invalid input, try again\n");
        }
        else
        {
            if(path(mat, u, v) == TRUE)
            {
                printf("there is a path from %d to %d\n", u, v);
            }
            else
            {
                printf("there is no path from %d to %d\n", u, v);
            }
        }
    }
    return 0;
}
