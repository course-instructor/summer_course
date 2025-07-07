#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "set.h"

#define MAX_LINE 256
#define MAX_LINES 100
enum STATUS{FAIL,SUCCES};

typedef unsigned int set[4];/* we create the sets with zeros*/

set SETA = {0};
set SETB = {0};
set SETC = {0};
set SETD = {0};
set SETE = {0};
set SETF = {0};

/**
 * @brief Get the set by name object
 * 
 * @param name the string of the set's name
 * @return it return a pointer to the correct set
 */

set *get_set_by_name(char *name) {
    if (strcmp(name, "SETA") == 0) 
    {
        return &SETA;
    }
    if (strcmp(name, "SETB") == 0) 
    {
        return &SETB;
    }
    if (strcmp(name, "SETC") == 0)
    { 
        return &SETC;
    }
    if (strcmp(name, "SETD") == 0)
    {
        return &SETD;
    } 
    if (strcmp(name, "SETE") == 0) 
    {
        return &SETE;
    }
    if (strcmp(name, "SETF") == 0) 
    {
        return &SETF;
    }
    return NULL;
}

/**
 * @brief this prints the user friendly menu
 * 
 */

void printmenu()
{
    printf(
                "1. ready_set: enter the set name and numbers separated by ','\n"
                "2. print_set: enter the set name\n"
                "3. union_set: enter set a, set b, set dest\n"
                "4. intersect_set: enter set a, set b, set dest\n"
                "5. sub_set: enter set a, set b, set dest\n"
                "6. symdiff_set: enter set a, set b, set dest\n"
                "7. stop\n"
            );
}

/**
 * @brief this function checks if the set that we got exits in our program
 * 
 * @param set_id 
 * @return int 
 */

int check_set_input(set * set_id)
{
    if(*set_id == NULL)
    {
        printf("Undefined set name\n");
        return FAIL;
    }
    else
    {
        return SUCCES;
    }
}

/**
 * @brief this function checks if we can add a number to our set
 * if its already in the set or if the number is not between 0 to 127 we cant add it
 * 
 * @param set 
 * @param num 
 * @return int 
 */

int check_num_to_add(set *set, int num)
{
    int retval = 1;
    int is_num_in_set = check_num_in_set(set,num);/* we check here if the number is already in the set */
    if(is_num_in_set)
    {
        if(num >= 0 && num < 128)/* if its not in the set so we check if its in the correct range */
        {
            retval = SUCCES;
        }
        else
        {
            printf("Invalid set number - Value out of range\n");
            retval = FAIL;
        }
    }
    else
    {
        printf("List of members is not terminated correctly\n");
        retval = FAIL;
    }
    return retval;
}

/**
 * @brief this function checks if the number we got is already in the set
 * 
 * @param set this is a pointer to the set we want to add the number into
 * @param num this is the number we try to add
 * @return int we return SUCCES if its not in the set and FAIL otherwise
 */

int check_num_in_set(set *set, int num)
{
    int index; /* will represent the index in the big array of the number*/
    int bit; /* will represent th correct bit in the correct index of the array*/
    index = num / 32;
    bit = num % 32;
    if(*(*set + index) & (1ul<<bit))
    {
        return FAIL;
    }
    else
    {
        return SUCCES;
    }

}

/**
 * @brief this function checks all the rules that refers to commas
 * 
 * @param input we reciecve the whole command(one line at a time)
 * @return int we return SUCCES if everything is fine and FAIL otherwise
 */

int check_commas(char* input)
{
    int retval = SUCCES;
    int comma_flag = 0;
    int comma_count = 0;
    while (isalnum(*input) || *input == '_') /* we skip the command */
    {
        input++;
    }
    while(isspace(*input)) /* we skip any kind of space */
    {
        input++;
    }
    if(*input == ',')/* if we get after the command a comma its a FAIL*/
    {
        printf("Illegal comma\n");
        retval = FAIL;
        input++;
    }
    else
    {
        while (*input && retval)
        {
            if (*input == ',')/* when we get to a comma we want to check if there was another one before that*/
            {
                if (comma_flag)/* we check if there was another comma before this one */
                {
                    printf("Multiple consecutive commas\n");
                    comma_flag = 0;
                    retval = FAIL;
                    
                }
                else /* if there was not a comma before its fine we just turn to flag to indicate that now we seen a coma*/
                {
                    comma_flag = 1;
                    input++;

                }
                comma_count++;/* we will check if there are commas as one of rules*/
            }
            else if (isspace(*input)) /* if it just spaces we ignore them but we still can get two commas in a row so we just move forward*/
            {
                input++;
            }
            else
            {
                input++;
                comma_flag = 0;
            }
        }

        
        if (comma_flag) /* at the of the line we want to check if the last char is comma*/
        {
            printf("Extraneuos text after end of command\n");
            retval = FAIL;
        }
        else if(!(comma_count)) /* if there was no commas in the line we return a FAIL*/
        {
            printf("Missing comma\n");
            retval = FAIL;
        }

        return retval;

    }
        
    
}

int check_commas_for_print(char* input)
{
    int retval = SUCCES;
    int comma_flag = 0;
    while (isalnum(*input) || *input == '_') /* we skip the command */
    {
        input++;
    }
    while(isspace(*input)) /* we skip any kind of space */
    {
        input++;
    }
    if(*input == ',')/* if we get after the command a comma its a FAIL*/
    {
        printf("Illegal comma\n");
        retval = FAIL;
        input++;
    }
    else
    {
        while (*input && retval)
        {
            if (*input == ',')/* when we get to a comma we want to check if there was another one before that*/
            {
                if (comma_flag)/* we check if there was another comma before this one */
                {
                    printf("Multiple consecutive commas\n");
                    comma_flag = 0;
                    retval = FAIL;
                    
                }
                else /* if there was not a comma before its fine we just turn to flag to indicate that now we seen a coma*/
                {
                    comma_flag = 1;
                    input++;
                }
                
            }
            else if (isspace(*input)) /* if it just spaces we ignore them but we still can get two commas in a row so we just move forward*/
            {
                input++;
            }
            else
            {
                input++;
                comma_flag = 0;
            }
        }

        
        if (comma_flag) /* at the of the line we want to check if the last char is comma*/
        {
            printf("Extraneuos text after end of command\n");
            retval = FAIL;
        }
        return retval;

    }
}

/**
 * @brief this function checks if we have all of the required parameters
 * 
 * @param set_addr1 a pointer to the first set
 * @param set_addr2 a pointer to the second set
 * @param set_addr3 a pointer to the third set
 * @return int we return SUCCES if we have all the params anf FAIL otherwise
 */

int check_num_params( set *set_addr1, set *set_addr2, set *set_addr3)
{
    int retval = SUCCES;
    if(!(set_addr1 != NULL && set_addr2 != NULL && set_addr3 != NULL ))
    {
        printf("Missing params\n");
        retval = FAIL;
    }
    return retval;
}

int main()
{
    printf("HELLO THERE!! :) IM READY, PLEASE PICK A FUNCTION\n");
    printmenu();/* printing the user friendly menu*/

    char input[MAX_LINE];
    int retval = SUCCES;

    while(retval)
    {
        while (fgets(input, MAX_LINE, stdin))
        {
            
            input[strcspn(input, "\n")] = '\0';

            char input_copy[MAX_LINE];/* we make a copy of the command to do checks on it*/
            strcpy(input_copy, input);

            

            char *command;
            char *token;

            token = strtok(input, " \t, ");
            if (token == NULL)
                continue;

            command = token;/* here we save just the command, the function name */
            if(strcmp(command,"print_set") == 0)
            {
                if(check_commas_for_print(input_copy))
                {
                    char *setname = strtok(NULL," \t, ");
                    set *set_id = get_set_by_name(setname);/* we get the set addres from the string */
                    if(!check_set_input(set_id))/* we check if there is a set that we got*/
                    {
                        printf("%s\n", input_copy);
                        printf("Undefined set name\n");
                        
                    }
                    print_set(*set_id);

                }
                

            }
            else if(check_commas(input_copy))/* for all the functions excluding print we need to check staff with commas*/
            {
                if(strcmp(command,"ready_set") == 0)
                {
                    
                    
                        char *setnsame = strtok(NULL," \t, ");
                        set *set_id = get_set_by_name(setnsame);
                        if(!check_set_input(set_id))/* we check if there is a set that we got*/
                        {
                            printf("%s\n", input_copy);
                            printf("Undefined set name\n");
                            continue;
                        }

                        token = strtok(NULL, " \t, ");/* we seperate each num */
                        while(token != NULL)
                        {
                            int flag_valid_input = SUCCES;/* private flag to check if the input is a number*/
                            char *dup_token = token;
                            while(*dup_token && flag_valid_input) /* we check if the number is int */
                            {
                                if(!isdigit(*dup_token))
                                {
                                    printf("Invalid set member - not an integer\n");
                                    flag_valid_input = FAIL;
                                }
                                dup_token++;
                            }
                            if(!flag_valid_input)
                            {
                                break;/* if the input is not int we want to move on to the next command*/
                            }
                            else
                            {
                                int num = (int) strtol(token, NULL, 10);
                                if (!check_num_to_add(set_id,num))/* we check if we can add this number */
                                {
                                    printf("%s\n", input_copy);
                                    break;
                                }
                                add_to_set(*set_id, num);
                                token = strtok(NULL, " \t, ");

                            }
                            
                        }

                    
                    
                }
                
                else if(strcmp(command,"union_set") == 0)
                {
                    
                    char* set1 = strtok(NULL," \t, ");
                    char *set2 = strtok(NULL," \t, ");
                    char *set3 = strtok(NULL," \t, ");
                    if (!(!set1 || !set2 || !set3)) /* we check if we got three paramters */
                    {
                        set *set_id1 = get_set_by_name(set1);
                        set *set_id2 = get_set_by_name(set2);
                        set *set_id3 = get_set_by_name(set3);
                        if(!(*set_id1 && *set_id2 && *set_id3))/* we check if there is a set that we got*/
                        {
                            printf("%s\n", input_copy);
                            printf("Undefined set name\n");
                            continue;
                        }
                        union_set(*set_id1,*set_id2,*set_id3);
                    }
                    else
                    {
                        printf("Missing paramter\n");
                    }
                    
                }
                else if(strcmp(command,"intersect_set") == 0)
                {
                    char *set1 = strtok(NULL," \t, ");
                    char *set2 = strtok(NULL," \t, ");
                    char *set3 = strtok(NULL," \t, ");
                    
                    if (!(!set1 || !set2 || !set3)) /* we check if we got three paramters */
                    {
                        set *set_id1 = get_set_by_name(set1);
                        set *set_id2 = get_set_by_name(set2);
                        set *set_id3 = get_set_by_name(set3);
                        if(!(*set_id1 && *set_id2 && *set_id3))/* we check if there is a set that we got*/
                        {
                            printf("%s\n", input_copy);
                            printf("Undefined set name\n");
                            continue;
                        }
                        intersect_set(*set_id1,*set_id2,*set_id3);
                    }
                    else
                    {
                        printf("Missing paramter\n");
                    }
                }
                else if(strcmp(command, "sub_set") == 0)
                {
                    char *set1 = strtok(NULL," \t, ");
                    char *set2 = strtok(NULL," \t, ");
                    char *set3 = strtok(NULL," \t, ");
                    
                    if (!(!set1 || !set2 || !set3)) /* we check if we got three paramters */
                    {
                        set *set_id1 = get_set_by_name(set1);
                        set *set_id2 = get_set_by_name(set2);
                        set *set_id3 = get_set_by_name(set3);
                        if(!(*set_id1 && *set_id2 && *set_id3))/* we check if there is a set that we got*/
                        {
                            printf("%s\n", input_copy);
                            printf("Undefined set name\n");
                            continue;
                        }
                        sub_set(*set_id1,*set_id2,*set_id3);
                    }
                    else
                    {
                        printf("Missing paramter\n");
                    }
                }
                else if(strcmp(command, "symdiff_set") == 0)
                {
                    char* set1 = strtok(NULL," \t, ");
                    char *set2 = strtok(NULL," \t, ");
                    char *set3 = strtok(NULL," \t, ");
                    if (!(!set1 || !set2 || !set3)) /* we check if we got three paramters */
                    {
                        set *set_id1 = get_set_by_name(set1);
                        set *set_id2 = get_set_by_name(set2);
                        set *set_id3 = get_set_by_name(set3);
                        if(!(*set_id1 && *set_id2 && *set_id3))/* we check if there is a set that we got*/
                        {
                            printf("%s\n", input_copy);
                            printf("Undefined set name\n");
                            continue;
                        }
                        symdiff_set(*set_id1,*set_id2,*set_id3);
                    }
                    else
                    {
                        printf("Missing paramter\n");
                    }
                }
                else if(strcmp(command, "stop") == 0)
                {
                    stop();
                }
                else /* if the command we got is not in our menu of commands */
                {
                    printf("%s\n", input_copy);
                    printf("Undefined command name\n");
                }
            }

        }
    }
    return retval;
}




