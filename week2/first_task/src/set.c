#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "set.h"



void init_set(set_t * set_ptr)
{
    for(int i = 0; i < SET_SIZE; i++)
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
    int index_in_sets = num / 32;
    int bit_index = num % 32 ;

    set [index_in_sets] = (set [index_in_sets]) |  (1 << bit_index);
}

/**
     * @brief the function reads from stdin a set of numbers seperated by ',' and appends them to the set  until it reaches -1 then it stops reading
     * @param set the set that will be red from
     * @param items a string with the items of the set seperated by ',' and white spaces
*/
void read_set(set_t set, const char *items)
{
    int current_num = 0;
    int read_index;

    for(read_index = 0; (read_index < strlen(items))  && (items[read_index] != '-1') && (items[read_index] != EOF); read_index++)
    {
        if(!isspace(items[read_index]))
        {
            if(items[read_index] == ',')
            {
                if(current_num < 128 && current_num > 0)
                {
                    add_item_to_set(set, current_num);
                    current_num = 0;
                }
                else
                {

                }   
            }

            else if(isdigit(items[read_index]))
            {
                current_num *= 0;
                current_num += items[read_index] - '0';
            }
            
            else
            {
                //error
            }
        }
    }
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
    
    for(arr_index = 0; arr_index < SET_SIZE; arr_index++)
    {
        for(bit_index = 0; bit_index < 32; bit_index++)
        {
            number = (set [arr_index] ) & (1 << (bit_index) );
            if(number)
            {
                printf("%d ", arr_index * 32 + bit_index);
                if(!(++ printed) % 16) //new line after every 16 numbers...
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
    for (int i = 0; i < SET_SIZE; i++) 
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
    for (int i = 0; i < SET_SIZE; i++) 
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
    for (int i = 0; i < SET_SIZE; i++) 
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
    for (int i = 0; i < SET_SIZE; i++) 
    {
        result[i] = (first[i] & ( ~ second[i])) | (second[i] & ( ~ first[i]));
    }
}