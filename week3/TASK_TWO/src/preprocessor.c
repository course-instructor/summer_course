#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <unistd.h>
#include <limits.h>

#include <../headers/preprocessor.h>

/**
 * @brief check if the character is the start of a comment
 *
 * @param chr the last character read that we want to check (check for '/')
 * @param file the file pointer that the second char will be red from (the '*' char)
 * @return boolean_e
 */
boolean_e check_comment_start(char chr, FILE *file)
{
    boolean_e ret = FALSE;
    if (chr == '/')
    {
        chr = fgetc(file);

        if (chr == '*')
        {
            ret = TRUE;
        }
        else
        {

            if (fseek(file, -1, SEEK_CUR))
            {
                perror("Error on fseek in check_comment_start");
            }
        }
    }
    return ret;
}

/**
 * @brief  check if the character is the end of a comment
 *
 * @param chr the last character read that we want to check (check for '*')
 * @param file the file pointer that the second char will be red from (the '/' char)
 * @return boolean_e
 */
boolean_e check_comment_end(char chr, FILE *file)
{
    boolean_e ret = FALSE;
    if (chr == '*')
    {
        chr = fgetc(file);

        if (chr == '/')
        {
            ret = TRUE;
        }
        else
        {
            if (fseek(file, -1, SEEK_CUR))
            {
                perror("Error on fseek in check_comment_end");
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
char * remove_comments(char *c_file_name)
{
    char *c1_file_name = malloc(strlen(c_file_name) + 1);
    FILE *c_file;
    FILE *c1_file;

    char chr;
    boolean_e in_comment = FALSE;

    strcpy(c1_file_name, c_file_name);
    strcat(c1_file_name, "1");

    c_file = fopen(c_file_name, "r");
    c1_file = fopen(c1_file_name, "w");

    if (!c_file)
    {
        perror("Error opening c_file\n");
    }

    else if (!c1_file)
    {
        perror("Error opening c1_file\n");
    }

    else
    {
        while ((chr = fgetc(c_file)) != EOF)
        {
            if (!in_comment)
            {
                if (check_comment_start(chr, c_file))
                {
                    in_comment = TRUE;
                }
                else
                {
                    fputc(chr, c1_file);
                }
            }

            else
            {
                if (check_comment_end(chr, c_file))
                {
                    in_comment = FALSE;
                }
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
 */
void find_header(char *line, char *ret)
{
    char file_name[FILE_NAME_LENGTH];
    char temp[FILE_NAME_LENGTH];

    if (sscanf(line, "#include \"%[^\"]\"", temp) == 1)
    {
        if (!realpath(temp, file_name))
        {
            printf("%s\n", temp);
            perror("error realpath in find_header");
            file_name[0] = '\0';
        }
    }
    else
    {
        file_name[0] = '\0';
    }
    strncpy(ret, file_name, FILE_NAME_LENGTH);
    ret[FILE_NAME_LENGTH - 1] = '\0';
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

    header_path = strdup(remove_comments(header_path));
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
        find_header(line, header_path);
        /* printf("header: %s\n", header_path); */
        if (*header_path)
        {
            process_header(c2_file, header_path);
        }
        else
        {
            /* printf("%s", line); */
            fwrite(line, sizeof(char), strlen(line), c2_file);
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
void turn_c1_to_c2(char *c1_file_name)
{
    char c2_path[LINE_LENGTH];
    FILE *c2_file;
    FILE *c1_file;

    if (!hcreate(HASH_SIZE))
    {
        printf("error creating hash");
    }
    else
    {
        strcpy(c2_path, "c2");

        c1_file_name = strdup(remove_comments(c1_file_name));
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

int main(int argc, char *argv[])
{
    char *c_file_name;
    if (argc != 2)
    {
        printf("need one parametr\n");
        return 1;
    }
    c_file_name = argv[1];

    turn_c1_to_c2(c_file_name);
    return 0;
}
