#include<stdio.h>
#include "set.h"

void initialize_set(set *s)
{
    (*s)[0] = 0;
    (*s)[1] = 0;
}
/**
 * @brief Insert numbers into a set
 */
void read_set(set** s,int * numbers)
{   
    (**s)[0] = 0;
    (**s)[1] = 0;
    int count = 0;
    while (numbers[count] != -1)
    {
        (**s)[count >= 64] |= (1ll<<numbers[count]);
        count++;
    }
}
/**
 * @brief print set
 */
void print_set(set** s)
{
    int i = 0;
    printf("[");
    
    for (i = 0; i < 128; i++)
    {   
        if((**s)[i >= 64] & (1ll<<i)){
            printf(" %d ", i );
            i++;
            break;
        }
    }
    for (; i < 128; i++)
    {   
        if((**s)[i >= 64] & (1ll<<i)){
            printf(", %d ", i );
        }
    }

    
    printf("]\n");
}
/**
 * @brief combines set A and set B
 */
void union_set(set ** s)
{
    set *set_a = s[0];
    set *set_b = s[1];
    set *set_c = s[2];
    (*set_c)[0] = (*set_a)[0] | (*set_b)[0];
    (*set_c)[1] = (*set_a)[1] | (*set_b)[1];
    

}
/**
 * @brief Numbers in set A and set B
 */
void intersect_set(set **s)
{
    set *set_a = s[0];
    set *set_b = s[1];
    set *set_c = s[2];
    (*set_c)[0] = (*set_a)[0] & (*set_b)[0];
    (*set_c)[1] = (*set_a)[1] & (*set_b)[1];

}
/**
 * @brief Numbers in set A that are not in set B
 */
void sub_set(set **s)
{
    set *set_a = s[0];
    set *set_b = s[1];
    set *set_c = s[2];
    (*set_c)[0] = (*set_a)[0] ^ ((*set_a)[0] & (*set_b)[0]);
    (*set_c)[1] = (*set_a)[1] ^ ((*set_a)[1] & (*set_b)[1]);


}
/**
 * @brief Numbers that are only in set A or only in set B
 */
void symdiff_set(set **s)
{
    set *set_a = s[0];
    set *set_b = s[1];
    set *set_c = s[2];
    (*set_c)[0] = ((*set_a)[0] | (*set_b)[0]) ^ ((*set_a)[0] & (*set_b)[0]);
    (*set_c)[1] = ((*set_a)[1] | (*set_b)[1]) ^ ((*set_a)[1] & (*set_b)[1]);

}
