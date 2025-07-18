#include <stdio.h>

#define SET_LENGTH 4
typedef unsigned int set[SET_LENGTH];

/**
 * @brief this function adds a number to the set
 * @param set this represents the set we add the number into
 * @param num the number we add to the set
 * @return we return a val if the function completed succfully or not
 */

int add_to_set(set set, int num)
{
    int retval;
    int index; /* will represent the index in the big array of the number*/
    int bit; /* will represent th correct bit in the correct index of the array*/
    index = num / 32;
    bit = num % 32;
    *(set + index) |= (1ul<<bit);/* we will move the bit according to the bit that represents the number in the correct ondex of the array */
    if(*(set + index)!= 0)
    {
        retval = 1;
    }
    else
    {
        retval = 0;
    }
    return retval;
}
/**
 * @brief this function will print the values in the set
 * @param set this represents the set we want to print
 */
void print_set(set set)
{
    int bit_pos;
    int index = 0;
    int counter_line = 0;
    int number;
    for(number = 0; number < 128; number++)/*this will represent the correct number*/
    {
        index = number / 32;
        bit_pos = number % 32;
        if(*(set + index) & (1ul<<bit_pos))/* if the bit that represents the number is on, we print the number */
        {
            if(counter_line < 15)/* if we printed less than 16 numbers we will print in the same line */
            {
                printf("%d ",number);
                counter_line++;
            }
            else/* if we already printed 16 numbers we will write to a new line */
            {
                printf("\n");
                counter_line = 0;
            }
        }

    }
    printf("\n");
}

/**
 * @brief this function adds two sets together into a third set
 * 
 * @param seta the first set we unite from
 * @param setb the second set we unite from
 * @param newset the set we will store the resault in
 */

void union_set(set seta, set setb, set newset)
{
    unsigned int index_in_set;
    for(index_in_set = 0; index_in_set < SET_LENGTH; index_in_set++)
    {
        *(newset + index_in_set) = (*(seta + index_in_set) | *(setb + index_in_set));/* we will make or so the all bits that are on in at least 
        one of the sets is on */
    }
}

/**
 * @brief this function will make a set that represents only the values that apeear in set a and set b
 * @param seta first set of the intersect 
 * @param setb second set in the intersect
 * @param newset the set that stores the intesection
 */

void intersect_set(set seta, set setb, set newset)
{ 
    int index_in_set;
    for(index_in_set = 0; index_in_set < SET_LENGTH; index_in_set++)
    {
        *(newset + index_in_set) = (*(seta + index_in_set) & *(setb + index_in_set));/*for each index we will put the value of the and bitwise of the sets*/
    }
}


/**
 * @brief this function puts in the third set all the values that apeear in set a and not in set b
 * @param seta the first set of the substraction
 * @param setb thesecond set of the substruction
 * @param newset this set will store the resault
 */
void sub_set(set seta, set setb, set newset)
{
    int index_in_set;
    
    for(index_in_set = 0; index_in_set < SET_LENGTH; index_in_set++)
    {
        *(newset + index_in_set) = (*(seta + index_in_set) & (~ *(setb + index_in_set)));
    }
}

/**
 * @brief this function makes a set that will include all the values 
 * that appear in set a or set b but NOT in both of them
 * 
 * @param seta the first set
 * @param setb the second set
 * @param newset this set will store the resault of the function
 */

void symdiff_set(set seta, set setb, set newset)
{
    int index_in_set;
    set united_set; 
    set intersected;
    intersect_set(seta,setb,intersected); /* this will represent the set of the numbers that appeat in both a and b*/
    union_set(seta,setb,united_set); /* this will represent the set of the numbers that apeear in a or b or both */
    for(index_in_set = 0; index_in_set < SET_LENGTH; index_in_set++)
    {
        *(newset + index_in_set) = *(united_set + index_in_set) & (~ *(intersected + index_in_set));/* we will take the union and "remove" from them
                                                                                                        the intersected sets*/
    }
}

/**
 * @brief this function stops the program
 * 
 */
void stop()
{
    exit(0);
}



