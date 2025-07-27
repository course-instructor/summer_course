#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "set.h"

#define G_SETS_COUNT 6
#define SET_NAME_LENGTH 5

enum set_error_e 
{
    SET_NAME, COMMAND_NAME,
    MEMBER_RANGE, MEMBER_TYPE,
    TEXT_AFTER_END, NO_TERMINATION,
    MISSING_PARAM,
    MULTIPLE_COMMAS, NO_COMMA, ILLIGALL_COMMA,
};

enum end_char_e
{
    NOT_ENDED, ERROR, COMMA, END_LINE, MISSING_COMMA,
};

typedef enum 
{
    FALSE, TRUE,
}bool_e;

set_t g_sets [G_SETS_COUNT] = {0};

char g_set_names [G_SETS_COUNT][SET_NAME_LENGTH] = 
{
    "SETA",
    "SETB",
    "SETC",
    "SETD",
    "SETE",
    "SETF",
};

void skip_spaces(int *char_index_pointer, char *line);
void get_set_members(int *char_index_pointer, char *line, set_t *set_ptr);
void init_all_sets(void);
int get_three_sets(set_t ** inp_sets_arr_pointer, int * char_index_pointer, char * line);

/**
 * @brief the function sends error messeges based on the value of the e error enum
 * 
 * @param e the error enum that gets its messege
 */
void error(enum set_error_e e)
{
    const char *errors[] = {
        "Undefined set name",
        "Undefined command name",
        "Member out of range (0-127)",
        "Invalid member type",
        "Extraneous text after end of command",
        "Missing parameter",
        "Multiple consecutive commas",
        "Missing comma",
        "Illegal comma",
        "Missing end of command (e.g., -1)"
    };
    printf("Error: %s\n", errors[e]);
}

/**
 * @brief the function reads a line of input char by char ends it with '\0' allocates it and returns it...
 * 
 * @return char* the line of input that was red
 */
char *get_line(void)
{
    int capacity = 10;
    int index = 0;
    char *str = malloc(capacity);
    char *temp;

    if (!str) 
    {
        printf("error: malloc failed\n\n");
    } 
    else 
    {
        int ch; // to detect EOF
        while ((ch = fgetc(stdin)) != EOF && ch != '\n' && str)
        {
            if (index >= capacity - 1) // leave space for '\0'
            {
                capacity *= 2; 
                temp = realloc(str, capacity); // try to double the space...
                if (!temp) //if allocation faild
                {
                    perror("realloc failed");
                    free(str);
                    str = NULL;
                }
                else
                {
                    str = temp;
                }
            }
            if(str)
            {
                str[index++] = (char)ch;
            }
        }

        if (str) 
        {
            str[index] = '\0';
        }
    }

    return str;
}


/**
 * @brief the function get_set looks through a string from a certian index and finds a set, it reads char by char
 * and it continues until the end of the line or the next text that isnt a blank space, it incriments the ponter 
 * and it can detect some errors in reading like multiple commas, and also returns an enum that helps diagnose
 * how it ended reading
 * 
 * @param char_index_pointer a pointer to the index from wich the fnction should start reading
 * @param line a pointer to the start of the line from wich the function will read the set
 * @param set_ptr_ptr a pointer to a pointer to a set in order to change the pointer of the set if is found
 * @return enum e_ended_with an enum that shows how did the reading end 
 */
enum e_ended_with get_set(int * char_index_pointer, char * line, set_t * * set_ptr_ptr)
{
    int start_index;
    int err_flag = 0;
    enum e_ended_with end_status = NOT_ENDED;
    int space_flag = 0;
    int got_set_flag = 0;

    int set_name_len = 0;


    int set_index = -1;

    skip_spaces( char_index_pointer, line); //skip emty spaces


    start_index = * char_index_pointer;

    if(line[* char_index_pointer] == ',') // if there was a comma before the set there were multiple commas...
    {
        end_status = ERROR;
        error(MULTIPLE_COMMAS);
    }

    //get the set
    for(; (* char_index_pointer)  < strlen(line) && (end_status == NOT_ENDED); (* char_index_pointer) ++)
    {
        if(line[(* char_index_pointer) ] == ',')
        {
            end_status = COMMA;
            
        }

        else if(line[(* char_index_pointer) ] == '\n' || line[(* char_index_pointer) ] == '\0')
        {
            end_status = END_LINE;
            
        }
        
        else if(isspace(line[(* char_index_pointer) ]))
        {
            space_flag = 1;
        }

        else if(space_flag) //if there was a space but no comma
        {
            end_status = MISSING_COMMA;
        }

        else // normal chars...
        {
            set_name_len  ++; //means its still reading and there wasnt an error...
        }
    }

    if ((line[*char_index_pointer] == '\0') && (end_status == NOT_ENDED))
    {
        end_status = END_LINE;
    }
    

    if(4 != set_name_len && end_status != COMMA) // set names are 4 chars long...
    {
        end_status = ERROR;
        error(SET_NAME);
    }

    else if(end_status == END_LINE || end_status == COMMA  )
    {
        for(int i = 0; i < G_SETS_COUNT && set_index == -1; i++)
        {
            if(0 == (strncmp(g_set_names[i], &line[start_index] , 4  )))
            {
                set_index = i;
            }
        
        }

        if(set_index == -1)//set wasnt found
        {
            end_status = ERROR;
            error(SET_NAME);
        }

        else
        {
            * set_ptr_ptr = & g_sets[set_index];
        }
    }
    return (end_status);
}


/**
 * @brief Get the three sets function is a template that uses the get_set function and is checking all of the error 
 * that can ocur between the reading of each set like commas
 * 
 * @param inp_sets_arr_pointer a pointer to an array of sets that will get assigend sets (SETA - SETF) if there arent any errors
 * @param char_index_pointer a pointer to the index from wich the fnction should start reading
 * @param line a pointer to the start of the line from wich the function will read the set
 * @return int returns wether there was an error or not (0 - no error, 1 - there was an error)
 */
int get_three_sets(set_t ** inp_sets_arr_pointer, int * char_index_pointer, char * line)
{

    int err_flag = 0;


    enum e_ended_with end_status = COMMA;
    int i = 0;

    for(; (i < 3) && (end_status == COMMA); i ++)
    {
        end_status = get_set(char_index_pointer,line,  ( inp_sets_arr_pointer + i));
    }

    if(end_status == MISSING_COMMA  )
    {
        err_flag = 1;
        
        if(i == 3) /* if all sets were red but the line hasnt ended */
        {
            error(TEXT_AFTER_END);
        }
        else /* if not all sets red... */
        {
            error(NO_COMMA);
        }
    }

    else if(end_status != END_LINE)
    {
        err_flag = 1;
    }

    else if(i < 3)
    {
        err_flag = 1;
        error(MISSING_PARAM);
    }

    else if(end_status == COMMA)
    {
        err_flag = 1;
        error(ILLIGALL_COMMA);
    }

    return(err_flag);
}

void skip_spaces(int * char_index_pointer, char * line)
{
    while (*char_index_pointer < strlen(line) && isspace(line[* char_index_pointer]) && line[* char_index_pointer] != '\n')
    {
        (* char_index_pointer) ++;
    }
}

/**
 * @brief Get the set members function reads set memers from a string (one line) by reading each char and
 * adding each number to a temp set it checks for errors if there arent any it copys the temp set to the
 * one that was sent to it
 * 
 * @param char_index_pointer a pointer to the index from wich the fnction should start reading
 * @param line a pointer to the start of the line from wich the function will read the set
 * @param set_ptr a pointer to the set that the caller wanted to add members to...
 */

void get_set_members(int * char_index_pointer, char * line, set_t * set_ptr)
{

    enum e_ended_with end_status = NOT_ENDED;

    set_t temp_set = {0};
    int number_added = 0;



    //read the numbers
    while(end_status != END_LINE && end_status != ERROR)
    {
        end_status = NOT_ENDED;
        int number = -2;
        int space_flag = 0;


        skip_spaces( char_index_pointer, line); //skip emty spaces

        if(line[(* char_index_pointer) ] == ',')
        {
            end_status = ERROR;
            error(MULTIPLE_COMMAS);
        }
        

        //read one number
        for(; (* char_index_pointer)  < strlen(line) && end_status == NOT_ENDED ; (* char_index_pointer) ++)
        {
            if(line[(* char_index_pointer) ] == ',')
            {
                end_status = COMMA;
            }

            else if(isspace(line[(* char_index_pointer) ]))
            {
                space_flag = 1;
            }

            else if(space_flag) //if there was a space but no comma
            {
                end_status = ERROR;
                error(NO_COMMA);
            }

            else if(line[(* char_index_pointer) ] == '-' && number == -2)
            {
                if(strlen(line) > (* char_index_pointer)  + 1)
                {
                    if(line[ ++ (* char_index_pointer )  ] == '1')
                    {
                        end_status = END_LINE;
                    }
                }

                if(end_status != END_LINE) // if it wasnt '-1'
                {
                    end_status = ERROR;
                    error(NO_TERMINATION);
                }
            }

            else if(isdigit(line[(* char_index_pointer) ])) // if the char is a number
            {
                if(number == -2)
                {
                    number = 0;
                }
                number *= 10;
                number += line[(* char_index_pointer) ] - '0';
            }

            else if(line[(* char_index_pointer) ] == '\n') //if line or file ended before finding -1
            {
                end_status = ERROR;
                error(NO_TERMINATION);
            }

            else // not acceptible char...
            {
                end_status = ERROR;
                error(MEMBER_TYPE);
            }
        }

        if(number >= 0 && number <= 127 && end_status != ERROR ) //if number in range add it to the temp set...
        {
            add_item_to_set(temp_set, number);
            number_added = 1;  
        }

        else if(end_status == END_LINE)// if -1 was entered...
        {
            skip_spaces(char_index_pointer, line); // skip empty spaces

            if(line[*char_index_pointer] != '\n' && line[*char_index_pointer] != '\0') //if there was anything other than spaces after '-1'
            {
                end_status = ERROR;
                error(TEXT_AFTER_END);
            }
        }

        else if(*char_index_pointer == strlen(line) && end_status != ERROR)
        {
            end_status = ERROR;
            error(NO_TERMINATION);
        }
        
        

        else if(end_status != ERROR)
        {
            end_status = ERROR;
            error(MEMBER_RANGE);
        }
    }

    if(end_status != ERROR && number_added) // if the temp set isn't empty and there wasn't error
    {
        

        for (int i = 0; i < SET_ARRAY_COUNT; i++)//change the original set to the temp
        {

            
            (*set_ptr)[i] = temp_set[i]; //(copy part by part)
        }
    }

}


/**
 * @brief the function initiates sets from SETA to SETF
 * 
 */
void init_all_sets(void)
{
    for (int i = 0; i < G_SETS_COUNT; i++)
    {
        init_set(& g_sets[i]);
    }
    
}

/**
 * @brief this function reads the input and its main functionality is to read the action that the user wants
 * to perform and then handeling the ending checks for the functions that dont read three sets (read_set, print_set)
 * and calling the library logical functions if there were no errors detected
 */
void set_main()
{

    init_all_sets();

    char * line;    



    int exit_flag = 0;

    printf("hello theese are valid commands: read_set, print_set, union_set, intersect_set, sub_set, symdiff_set, stop\n");


    while (!exit_flag)
    {
        int char_index = 0;
        bool_e line_error_found = FALSE;
        int param_index = 0;    
        int line_length =  0;



        line = get_line();
        if(!line) //alocation failed...
        {
            line_error_found = TRUE;
        }

        line_length = strlen(line);

        for( ; char_index < line_length && !line_error_found && ! isspace(line[char_index]); char_index++)
        {
            if(line[char_index] == ',')
            {
                error(ILLIGALL_COMMA);
                line_error_found = TRUE;
            }
        }

        if(!line_error_found)
        {
            char * action = malloc(line_length + 1);
            strncpy(action,line, char_index );
            action[char_index] = '\0';

            skip_spaces(&char_index, line);
            if(line[char_index] == ',')
            {
                line_error_found = TRUE;
                error(ILLIGALL_COMMA);
            }

            if(strncmp(action, "read_set", char_index) == 0 && !line_error_found) 
            {
                if(line[char_index] == ',')
                {
                    line_error_found = TRUE;
                    error(ILLIGALL_COMMA);
                }
                else
                {
                    int start_index = char_index;
                    set_t * set_ptr = NULL;
                    enum e_ended_with end_status = get_set(&char_index, line, & set_ptr);
                    
                    if(end_status == ERROR)
                    {
                        line_error_found = TRUE;
                    }

                    else if(end_status  == END_LINE ) // if ended not with comma then there werent memeber parameters...
                    {
                        line_error_found = TRUE;
                        error(MISSING_PARAM);
                    }

                    else if(!line_error_found)
                    {
                        get_set_members(&char_index, line, set_ptr);
                    }
                }
            }
            
            else if(0 == (strncmp(action, "print_set",char_index)) && !line_error_found)
            {
                set_t * set_ptr;
                enum e_ended_with end_status = get_set(&char_index, line, & set_ptr);
                
                if (end_status == ERROR)
                {
                    line_error_found = TRUE;
                }

                else if(end_status == COMMA) // if ended with comma...
                {
                    line_error_found = TRUE;
                    error(ILLIGALL_COMMA);
                }

                else if(end_status == MISSING_COMMA )
                {
                    line_error_found = TRUE;
                    error(TEXT_AFTER_END);
                }
                
                else if(end_status == END_LINE && !line_error_found ) //if no error and only one set
                {
                    int not_empty_flag = 0;

                    for(int i = 0; i < SET_ARRAY_COUNT; i ++) //check if the set is empty...
                    {
                        if(*(set_ptr)[i] != 0) //atleast one part to not be empty...
                        {
                            not_empty_flag = TRUE;
                        }
                    }

                    if (not_empty_flag)
                    {
                        print_set(*set_ptr);
                    }
                    else
                    {
                        printf("set is empty\n");
                    }
                }
                
            }
            else if(0 == (strncmp(action, "union_set",  char_index)) && !line_error_found)
            {
                set_t * inp_sets [3] ;

                line_error_found = get_three_sets( inp_sets, &char_index, line);
                if(!line_error_found)
                {

                    union_set(inp_sets[0], inp_sets[1], inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action, "intersect_set", char_index)) && !line_error_found)
            {
                set_t * inp_sets [3];

                line_error_found = get_three_sets( inp_sets, &char_index, line);
                if(!line_error_found)
                {
                    intersect_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action,  "sub_set", char_index)) && !line_error_found)
            {
                set_t * inp_sets [3];

                line_error_found = get_three_sets( inp_sets, &char_index, line);
                if(!line_error_found)
                {
                    sub_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }                
            }
            else if(0 == (strncmp(action,  "symdiff_set", char_index)) && !line_error_found)
            {
                
                set_t * inp_sets [3];

                line_error_found = get_three_sets( inp_sets, &char_index, line);
                if(!line_error_found)
                {
                    symdiff_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action, "stop", char_index)) && !line_error_found)
            {
                skip_spaces(& char_index,line);
                if(line[char_index] != '\0' && line[char_index] != '\n')
                {
                    line_error_found = TRUE;
                    error(TEXT_AFTER_END);
                }
                else
                {
                    printf("exiting program\n");
                    exit_flag = 1;// end the program   
                }
            }

            else if(!line_error_found)
            {
                //uncknown action
                line_error_found = TRUE;
                error(COMMAND_NAME);
            }

            free(action); //after handeling the action we can free it...
        }

        free(line);//after handeling the action we can free it...
    }
}

int main()
{
    set_main();
    return(0);
}
