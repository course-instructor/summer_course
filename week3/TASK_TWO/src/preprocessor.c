#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <unistd.h>
#include <limits.h>

#include <../headers/preprocessor.h>

const char COMMENT_CHARS[] = {'/','*'};

/**
 * @brief check if the character is the starting a comment or ending a comment
 *
 * @param chr the last character read that we want to check 
 * @param file the file pointer that the second char will be red from
 * @return boolean_e TRUE if state changed ERROR if there was an error.
 */
boolean_e check_comment_state_change(char chr, FILE *file, boolean_e is_start)
{
    boolean_e ret = FALSE;
    /* the order to cpompare the chars to COMMENT_CHARS */
    int first, second ;

    if(is_start)
    {
        first = 0;
        second = 1;
    }
    else
    {
        first = 1;
        second = 0;
    }

   
    if (chr == COMMENT_CHARS[first])
    {
        chr = fgetc(file);

        if (chr == COMMENT_CHARS[second])
        {
            ret = TRUE;
        }
        else
        {

            if (fseek(file, -1, SEEK_CUR))
            {
                perror("Error on fseek in check_comment_state_change\n");
                ret = ERROR;
            }
        }
    }
    return ret;
}


/**
 * @brief reads the file and writes it without comments to a new file
 *
 * @param c_file_name the file to read from
 *
 * @return char* the name of the new file that was created with the comments removed
 */
char * remove_comments(char *c_file_name, boolean_e * success)
{
    char *c1_file_name = malloc(strnlen(c_file_name, FILE_NAME_LENGTH) + 2);
    FILE *c_file;
    FILE *c1_file;

    char chr;
    boolean_e in_comment = FALSE;

    strncpy(c1_file_name, c_file_name, strnlen(c1_file_name, FILE_NAME_LENGTH));
    strncat(c1_file_name, "1",2);

    c_file = fopen(c_file_name, "r");
    c1_file = fopen(c1_file_name, "w");

    *success = TRUE;

    if (!c_file)
    {
        perror("Error opening c_file\n");
        *success = FALSE;
    }

    else if (!c1_file)
    {
        perror("Error opening c1_file\n");
        *success = FALSE;
    }

    else
    {
        while ((chr = fgetc(c_file)) != EOF)
        {
            boolean_e state_change = check_comment_state_change(chr, c1_file, in_comment);

            if(state_change == TRUE)
            {
                in_comment = (in_comment)?(FALSE):(TRUE);
            }
            else if(state_change == ERROR)
            {
                break;
            }

            /* if not in comment and the state wasn't changed in this instance */
            if(!in_comment && !state_change)
            {
                fputc(chr, c1_file);
            }
        }
    }
    fclose(c_file);
    fclose(c1_file);
    return c1_file_name;
}

/**
 * @brief the function checks if the line is a header include line
 *
 * @param line the line to check
 * @param ret the header path if exists, otherwise an empty string
 * 
 * @return boolean_e TRUE if found include
 */
boolean_e preprocessor_check_for_include(char *line, char *ret)
{
    boolean_e success = TRUE;
    char file_name[FILE_NAME_LENGTH];
    char temp[FILE_NAME_LENGTH];

    if (sscanf(line, "#include \"%[^\"]\"", temp) == 1)
    {
        if (!realpath(temp, file_name))
        {
            printf("%s\n", temp);
            perror("error realpath in preprocessor_check_for_include");
            file_name[0] = '\0';
            success = ERROR;
        }
    }
    else
    {
        file_name[0] = '\0';
        success = FALSE;
    }
    strncpy(ret, file_name, FILE_NAME_LENGTH);
    ret[FILE_NAME_LENGTH - 1] = '\0';
    return(success);
}

/**
 * @brief this function processes a header file, checking if adding it will cause a storm
 *
 * @param c2_file the file to write the contents of the header to
 * @param header_path the path to the header file that we need to check and if needed copy to c2_file
 */
void process_header(FILE *c2_file, char *header_path)
{
    ENTRY entry;
    boolean_e success;
    header_path = strdup(remove_comments(header_path, &success));
    if(success ==   TRUE)
    {

        FILE *header_file = fopen(header_path, "r");

        entry.data = NULL;

        if (!header_file)
        {
            printf("error opening file: process_header\n");
        }
        else
        {
            ENTRY * temp;
            entry.key = strdup(header_path);
            /* printf("try %s\n", header_path); */

            temp = hsearch(entry, FIND);

            if (temp == NULL)
            {
                int *p = malloc(sizeof(int));
                *p = 1;
                entry.data = p;
                hsearch(entry,ENTER);
                /* printf("new %s\n", header_path); */
                process_file(c2_file, header_file);
            }
            else
            {
                int * p = (int*)(temp->data);
                if (*p == 0)
                {
                    *p = 1;
                    /* printf("again %s\n", header_path); */
                    process_file(c2_file, header_file);
                }
                else
                {
                    printf("storm detected! %s\n", header_path);
                }
            }

            fclose(header_file);

            temp = hsearch(entry, FIND);
            if (temp == NULL || temp->data == NULL)
            {
                printf("ERROR for %s\n", header_path);
            }
            else
            {
                int * p = (int*) temp->data;
                *p = 0;
                /* printf("done %s\n", temp->key); */
            }
        }
    }
}

/**
 * @brief the function processes the input file line by line, checking for header includes
 *
 * @param c2_file file to write to
 * @param input file to read from and if header proccess it otherwise write the line to c2_file
 */
void process_file(FILE *c2_file, FILE *input)
{
    char line[LINE_LENGTH];
    char header_path[FILE_NAME_LENGTH];



    header_path[0] = '\0';

    while (fgets(line, LINE_LENGTH, input))
    {
        boolean_e success = preprocessor_check_for_include(line, header_path);
        if(success == TRUE)
        {
            process_header(c2_file, header_path);
        }
        else if(success == FALSE)
        {
            /* printf("%s", line); */
            fwrite(line, sizeof(char), strnlen(line, LINE_LENGTH), c2_file);
        }
    }
    fputc('\n', c2_file);
    fflush(c2_file);
}

/**
 * @brief the function creates a file, and will write the input file into it without any includes
 *
 * @param c1_file_name the file to read from
 */
void preprocessor_run(char *c1_file_name)
{
    char c2_path[FILE_NAME_LENGTH];
    FILE *c2_file;
    FILE *c1_file;

    if (!hcreate(HASH_SIZE))
    {
        printf("error creating hash");
    }
    else
    {
        boolean_e success;
        c1_file_name = strdup(remove_comments(c1_file_name, &success));
        if(success == TRUE)
        {
                

            strncpy(c2_path, c1_file_name, FILE_NAME_LENGTH);
            int len = strnlen(c2_path, FILE_NAME_LENGTH);
            if (len > 0 && len < FILE_NAME_LENGTH) 
            {
                c2_path[len - 1] = '2';
                c2_path[len] = '\0';
            }

            c1_file = fopen(c1_file_name, "r");


            c2_file = fopen(c2_path, "w");

            if (!c1_file || !c2_file)
            {
                printf("error opening file in storm check\n");
            }

            else
            {
                process_file(c2_file, c1_file);
            }

            hdestroy();
        }
    }
}

int main(int argc, char *argv[])
{
    char *c_file_name;
    if (argc != 2)
    {
        printf("need one parametr\n");
        return 1;
    }
    c_file_name = argv[1];

    preprocessor_run(c_file_name);
    return 0;
}
