#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "set.h"

enum e_errors 
{
    SET_NAME, COMMAND_NAME,
    MEMBER_RANGE, MEMBER_TYPE,
    TEXT_AFTER_END, MISSING_PARAM,
    MULTIPLE_COMMAS, NO_COMMA, ILLIGALL_COMMA, NO_TERMINATION
};

enum e_ended_with
{
    NOT_ENDED, ERROR, COMMA, END_LINE, MISSING_COMMA,
};

set_t g_sets [6] = {0};

char g_set_names [6][5] = 
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

void error(enum e_errors e)
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
        for(int i = 0; i < 6 && set_index == -1; i++)
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
        

        for (int i = 0; i < SET_SIZE; i++)//change the original set to the temp
        {

            
            (*set_ptr)[i] = temp_set[i]; //(copy part by part)
        }
    }

}

void init_all_sets(void)
{
    for (int i = 0; i < 6; i++)
    {
        init_set(& g_sets[i]);
    }
    
}

void set_main()
{

    init_all_sets();

    char * line;    



    int exit_flag = 0;

    printf("hello theese are valid commands: read_set, print_set, union_set, intersect_set, sub_set, symdiff_set, stop\n");


    while (!exit_flag)
    {
        int char_index = 0;
        int err_flag = 0;
        int param_index = 0;


        line = get_line();
        if(!line) //alocation failed...
        {
            err_flag = 1;
        }

        for( ; char_index < strlen(line) && !err_flag && ! isspace(line[char_index]); char_index++)
        {
            if(line[char_index] == ',')
            {
                error(ILLIGALL_COMMA);
                err_flag = 1;
            }
        }

        if(!err_flag)
        {
            char * action = malloc(char_index + 1);
            strncpy(action,line, char_index );
            action[char_index] = '\0';

            skip_spaces(&char_index, line);
            if(line[char_index] == ',')
            {
                err_flag = 1;
                error(ILLIGALL_COMMA);
            }

            if(strncmp(action, "read_set", char_index) == 0 && !err_flag) 
            {


                if(line[char_index] == ',')
                {
                    err_flag = 1;
                    error(ILLIGALL_COMMA);
                }
                else
                {

                

                    int start_index = char_index;
                    set_t * set_ptr = NULL;
                    enum e_ended_with end_status = get_set(&char_index, line, & set_ptr);
                    
                    if(end_status == ERROR)
                    {
                        err_flag = 1;
                    }

                    else if(end_status  == END_LINE ) // if ended not with comma then there werent memeber parameters...
                    {
                        err_flag = 1;
                        error(MISSING_PARAM);
                    }

                    else if(!err_flag)
                    {
                        get_set_members(&char_index, line, set_ptr);
                    }
                }
            }
            
            else if(0 == (strncmp(action, "print_set",char_index)) && !err_flag)
            {
                set_t * set_ptr;
                enum e_ended_with end_status = get_set(&char_index, line, & set_ptr);
                
                if (end_status == ERROR)
                {
                    err_flag = 1;
                }

                else if(end_status == COMMA) // if ended with comma...
                {
                    err_flag = 1;
                    error(ILLIGALL_COMMA);
                }

                else if(end_status == MISSING_COMMA )
                {
                    err_flag = 1;
                    error(TEXT_AFTER_END);
                }
                
                else if(end_status == END_LINE && !err_flag ) //if no error and only one set
                {
                    int not_empty_flag = 0;

                    for(int i = 0; i < SET_SIZE; i ++) //check if the set is empty...
                    {
                        if(*(set_ptr)[i] != 0) //atleast one part to not be empty...
                        {
                            not_empty_flag = 1;
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
            else if(0 == (strncmp(action, "union_set",  char_index)) && !err_flag)
            {
                set_t * inp_sets [3] ;

                err_flag = get_three_sets( inp_sets, &char_index, line);
                if(!err_flag)
                {

                    union_set(inp_sets[0], inp_sets[1], inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action, "intersect_set", char_index)) && !err_flag)
            {
                set_t * inp_sets [3];

                err_flag = get_three_sets( inp_sets, &char_index, line);
                if(!err_flag)
                {
                    intersect_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action,  "sub_set", char_index)) && !err_flag)
            {
                set_t * inp_sets [3];

                err_flag = get_three_sets( inp_sets, &char_index, line);
                if(!err_flag)
                {
                    sub_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }                
            }
            else if(0 == (strncmp(action,  "symdiff_set", char_index)) && !err_flag)
            {
                
                set_t * inp_sets [3];

                err_flag = get_three_sets( inp_sets, &char_index, line);
                if(!err_flag)
                {
                    symdiff_set(*inp_sets[0], *inp_sets[1], *inp_sets[2]);
                }
            }
            else if(0 == (strncmp(action, "stop", char_index)) && !err_flag)
            {
                skip_spaces(& char_index,line);
                if(line[char_index] != '\0' && line[char_index] != '\n')
                {
                    err_flag = 1;
                    error(TEXT_AFTER_END);
                }
                else
                {
                    printf("exiting program\n");
                    exit_flag = 1;// end the program   
                }
            }

            else if(!err_flag)
            {
                //uncknown action
                err_flag = 1;
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
