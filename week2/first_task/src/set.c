#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "set.h"

#define PRINT_NEW_LINE 16

void init_set(set_t * set_ptr)
{
    for(int i = 0; i < SET_ARRAY_COUNT; i++)
    {
        (*set_ptr)[i] = 0;
    }
}

/**
 * @brief the function adds a number to the set
 * @param set the set to add num to
 * @param num the number to add to the set (0-127)s
 */
void add_item_to_set(set_t set, int num)
{
    int index_in_sets = num / SET_ARRAY_LENGTH;
    int bit_index = num % SET_ARRAY_LENGTH; ;

    set [index_in_sets] = (set [index_in_sets]) |  (1 << bit_index);
}



/**
 * @brief prints the values of the set in ascending order up to 16 in one line
 * @param set the set to be printed
 * 
*/
void print_set(const set_t set)
{
    int bit_index;
    int arr_index;
    int number;
    int printed = 0;
    
    for(arr_index = 0; arr_index < SET_ARRAY_COUNT; arr_index++)
    {
        for(bit_index = 0; bit_index < SET_ARRAY_LENGTH; bit_index++)
        {
            number = (set [arr_index] ) & (1 << (bit_index) );
            if(number)
            {
                printf("%d ", arr_index * SET_ARRAY_LENGTH + bit_index);
                if(!(++ printed) % PRINT_NEW_LINE) //new line after every 16 numbers...
                {
                    printf("\n");
                }
            }
            
        }
    }
    printf("\n");
}

/**
 * @brief the function performs a union action between two sets and stores the result in the third
 * @param first the first set
 * @param second the second set
 * @param result the set to store the result
 */
void union_set(set_t first, set_t second, set_t result)
{
    for (int i = 0; i < SET_ARRAY_COUNT; i++) 
    {
        (result)[i] = (first)[i] | (second)[i];
    }
}


/**
 * @brief the function performs a intersect action between two sets and stores the result in the third
 * @param first the first set
 * @param second the second set
 * @param result the set to store the result
 */

void intersect_set( set_t first, set_t second, set_t result)
{
    for (int i = 0; i < SET_ARRAY_COUNT; i++) 
    {
        result[i] = first[i] & second[i];
    }
}

/**
 * @brief the function subtracts the second set from the first and stores the result in the third
 * @param first the first set
 * @param second the second set
 * @param result the set to store the result
 */
void sub_set( set_t first,  set_t second, set_t result)
{
    for (int i = 0; i < SET_ARRAY_COUNT; i++) 
    {
        result[i] = (first[i] & ( ~ second[i]));
    }
}

/**
 * @brief the function performs a symetric subtraction action between two sets and stores the result in the third
 * @param first the first set
 * @param second the second set
 * @param result the set to store the result
 */
void symdiff_set( set_t first, set_t second, set_t result)
{
    for (int i = 0; i < SET_ARRAY_COUNT; i++) 
    {
        result[i] = (first[i] & ( ~ second[i])) | (second[i] & ( ~ first[i]));
    }
}