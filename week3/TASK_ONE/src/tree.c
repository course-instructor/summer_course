#include <stdio.h>
#include <ctype.h>
/* isspace */

#include "../headers/tree.h"


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
