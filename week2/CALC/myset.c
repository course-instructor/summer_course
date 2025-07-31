#define _GNU_SOURCE /*For strtok_r*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "set.h"
#include "myset.h"
#define BUF_SIZE 256
#define MAX_COMMAND_NAME_LENGTH 16
#define MAX_SET_NAME_LENGTH 4
#define COMMNAD_AMOUNT 7
#define SETS_AMOUNT 6
#define SUCCESS 1 
#define FALIURE 0
#define MAX_NUMBERS 128




/*Annotated -> something with metadata */
typedef struct  
{

    char * name;                        /*Command name*/
    unsigned int number_of_parameters;  /*Number of sets that command gets*/
    int is_numbers;                     /*Does command get numbers flag*/
    void (*operation)(void*,void*);     /*Function callback*/
} annotated_command;

typedef struct  
{
    set s;
    const char name[4]; /*Set name*/
} annotated_set;



static const annotated_command commands[COMMNAD_AMOUNT] = 
{   /* Name    parameters  is_numbers, function call */
    {"read_set"     ,   1   ,   1   ,(void*)&read_set     },
    {"print_set"    ,   1   ,   0   ,(void*)&print_set    },
    {"union_set"    ,   3   ,   0   ,(void*)&union_set    },
    {"intersect_set",   3   ,   0   ,(void*)&intersect_set},
    {"sub_set"      ,   3   ,   0   ,(void*)&sub_set      },
    {"symdiff_set"  ,   3   ,   0   ,(void*)&symdiff_set  },
    {"stop"         ,   0   ,   0   ,(void*)&stop         }
};
static annotated_set sets[SETS_AMOUNT] = 
{
    {.name = "SETA" },
    {.name = "SETB" },
    {.name = "SETC" },
    {.name = "SETD" },
    {.name = "SETE" },
    {.name = "SETF" }
};




int main(int argc , char ** args)
{
    startProgram(argc , args);
    return 0;
}
/**
 * @brief starts the conculator
 */
void startProgram(int argc , char ** args)
{
    int command_num = -1;
    set *varibles[3] = {NULL};
    int numbers[128];
    char * input_path = NULL;
    for (int i = 0; i < SETS_AMOUNT; i++)
    {
        initialize_set(&sets[i].s);
    }
    
    printInstructions();
    /* Input from file */
    if(argc == 2) 
    {
        input_path = args[1];
    }
    input(&command_num,varibles,numbers,input_path);
}
/**
 * @brief calls the callback operation of commands
 */
void operation(int command_num, set ** varibles, int * numbers)
{
    commands[command_num].operation(varibles,numbers);
}

int input(int * command_num, set * varibles[3], int numbers[128],char * path)
{
    char line[BUF_SIZE] = {0}; /* Current line */
    char tmp[BUF_SIZE]  = {0};
    char *next;        /* Next word in Current Line */
    int offset = 0;
    *command_num = -1;
    FILE * input_from;

    /*If file is given read from it , else read form console */
    if(path !=NULL){
        input_from = fopen(path,"r");
        if (input_from==NULL ) 
        {
            printf("Invalid file name\n");
            return FALIURE;
        } 
    }
    else
    {
        input_from = stdin ;

    }
    /*While reading lines*/
    printf("\n> ");
    while(fgets(line, BUF_SIZE, input_from) != NULL)
    {
        offset = 0;
        line[strcspn(line, "\n")] = 0;
        if(*line != '\n' && *line != '\0')
        {
            /*Print command if reading from file*/
            if(path!=NULL)
            {
                printf("[COMMAND] %s",line);
            }
            /* Check if at least one of the validation is worng */
            if(    commandValidation(line, command_num,&offset) 
                && setValidation(line,varibles,&offset,*command_num) 
                && (!commands[*command_num].is_numbers || numberValidation(line,&offset,numbers))
               // && commaValidation(tmp))
            )
            {
                printf("YEY\n");
               // operation(*command_num,varibles,numbers);
            }
       
        }
        memset(line,0,BUF_SIZE);
        printf("\n> ");

    }
    /*Close file if it was opened*/
    if(path!=NULL)
    {
        fclose(input_from);
    }
    
    return SUCCESS;

}
/**
 * @brief Prints the available commands and variables.
 */
void printInstructions(void)
{
    int i;
    printf("---- SET CALCULATOR ----\n");
    printf("\nCommands:\n");
    for (i = 0; i < COMMNAD_AMOUNT; i++)
    {
        printf("%d. %s\n" ,i+1,commands[i].name);
    }
    printf("\nVariables:\n");
    for (i = 0; i < SETS_AMOUNT; i++)
    {
        printf("%d. %s\n" ,i+1,sets[i].name);

    }
    printf("\n");
    
}
/**
 * @brief checks if command part of input is valid
 * @param line input string
 * @param command_num saves given command in here
 * @param p_offset pointer to offset of a string to know from which part of the string next to validate
 * @return returns 1 if valid or 0 if invalid
 */
int commandValidation(char * line,int * command_num, int * p_offset)
{
    int offset = *p_offset;
    * command_num = -1;
    int i;
    int status = SUCCESS;
    /*skip whitespaces */
    while(line[offset] != '\0' && isspace(line[offset]))
    {
        offset++;
    }
    
    for (i = 0; i < COMMNAD_AMOUNT && *command_num == -1; i++)
    {
        if(strncmp(commands[i].name,line+offset,strnlen(commands[i].name,MAX_COMMAND_NAME_LENGTH)) == 0)
        {
            
            *command_num = i;
            offset += strnlen(commands[i].name,MAX_COMMAND_NAME_LENGTH);
        } 
    }
    if(*command_num == -1)
    {
        printf("[ERROR] Undifined command name\n");
        status = FALIURE;
    }
    *p_offset = offset;
    return status;

}
/**
 * @brief Check if set name is valid 
 * @param line input string
 * @param varibles  array of sets
 * @param p_offset pointer to offset of a string to know from which part of the string next to validate
 * @param command_num  command number for knowing how many varibles to get
 */
int setValidation(char * line, set * varibles[3],int * p_offset,int command_num)
{
    int offset = *p_offset;
    int status = SUCCESS;
    unsigned int i,j = 0;
    /*skip whitspaces */
    while(line[offset] != '\0' && isspace(line[offset]))
    {
        offset++;
    }


    for (i = 0; i < commands[command_num].number_of_parameters; i++)
    {
        for (j = 0; j < SETS_AMOUNT; j++)
        {
            if(strncmp(sets[i].name,line+offset,strnlen(commands[i].name,MAX_COMMAND_NAME_LENGTH)) == 0)
            {
            varibles[i] = &sets[i].s;
            offset += strnlen(sets[i].name,MAX_SET_NAME_LENGTH);
            }    
              
        }

        if(varibles[i] == NULL)
        {
            printf("[ERROR] Undifined set name\n");
            status = FALIURE;
        }      
        
        
    }

    /*Check if input after end of program  (unless command expects numbers)*/
    if(commands[command_num].is_numbers == 0)
    {
        while(line[offset] != '\0' && status == SUCCESS)
        {
            if(isspace(line[offset]) == 0)
            {
                printf("[ERROR] Extraneous text after end of command\n");
                status = FALIURE;

            }
        }
          
    }
    *p_offset = offset;
    return status;
}
/**
 * @brief  Checks if numbers in input are valid
 * @param line input string
 * @param numbers output nunbers
 * @return  returns 1 if valid or 0 if invalid
 */
int numberValidation(char * line,int * p_offset, int numbers[128])
{
    int number_count = 0;
    const char *p = line;
    int num;
    int is_command_end = 0;
    int offset = *p_offset;
    while(line[offset] != '\0' && isspace(line[offset]))
    {
        offset++;
    }
    


    /*Numbers vaildiation*/
    if(line[offset] == '\0')
    {
        printf("[ERROR] missing paramter\n");
        return FALIURE;
    }
    while (*p) 
    {
        if (isdigit(*p) || (*p == '-' && isdigit(*(p + 1)))) 
        {
            char *end;
            int num = strtol(p, &end, 10);
            numbers[number_count++] = num;
            p = end; 
        } 
        else if(!isspace(line[offset]) && isdigit(line[offset]), !line[offset] == ',')
        {
            printf("[ERROR] Invalid set member - not an integer \n");
            return FALIURE;
        }
        else 
        {
            p++;
        }
    }


        

    
        num = strtol(line+offset,line+offset,10);
        
        is_command_end = num == -1;
        if(is_command_end)
        {
            if(line!=NULL && line[offset]!='\n')
            {
                while(line!=NULL && line[offset]!='\n')
                {
                   if(line[offset] != ' ' && line[offset] != '\t')
                    {
                        printf("Extraneous text after end of command\n");
                        return FALIURE;
                    }
                    line++;
                }
            }
        
        if(num < -1 || num > 127)
        {
            printf("[ERROR] Invalid set member - value out of range\n");
            return FALIURE;
        }
        numbers[number_count++] = num;
        word = strtok_r(NULL," ,\t\n",line);
 
    }
    if(!is_command_end)
    {
        printf("[ERROR] List of set numbers is not terminated correctly\n");
        return FALIURE;
    }
    
        
    
    return SUCCESS;
}
/**
 * @brief checks if commas in input are legal
 * @param line input string
 * @return  returns 1 if valid or 0 if invalid
 */
int commaValidation(char * line)
{
    char * next = line;
    int var_count = 0;
    int comma_count = 0; 
    int comma_flag = 0; /*Flag for checking if the last character was a comma */
    /*Check if there is a comma before the command*/
    while(next != NULL && (*next == ' ' || *next == '\t' ||  *next == ','))
    {
        if(*next == ',')
        {
            printf("[ERROR] Illegal comma\n");
            return FALIURE;
        }
        next++;
    }

    /*skip the command*/
    while(next != NULL && *next != ' ' && *next!= '\t' && *next != ',')
    {
        next++;
    }
        /*Check if there is a comma after the command*/
    while(next != NULL && (*next == ' ' || *next == '\t' || *next == ',' ))
    {
        
        if(*next == ',')
        {
            printf("[ERROR] Illegal comma\n");
            return FALIURE;
        }
        next++;
    }
    /*Check varible's commas*/
    while (next != NULL && *next !='\0' && *next !=EOF && *next !='\n')
    {
        if(*next == ',') 
        {
            if(comma_flag == 1)
            {
                printf("[ERROR] Multiple consecutive commas\n");
                return FALIURE;
            }
            
            comma_flag = 1;
            comma_count++;
        }  
        else if (isalnum(*next))
        {
            while( next != NULL && isalnum(*next))
            {
                next++;
            }
            var_count++;
            comma_flag = 0;
            continue;
        }
        next++;
    }
    /*Check if there more varibles then commas*/
    if(var_count>comma_count+1)
    {
        printf("[ERROR] Missing comma\n");
        return FALIURE;
    }
    return SUCCESS;

}

/**
 * @brief Exit program
 */
void stop(void)
{
    exit(SUCCESS);
}