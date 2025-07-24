#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <unistd.h>
#include <limits.h>

#include <../headers/preprocessor.h>

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

void remove_comments(char *c_file_name)
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
}

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

boolean_e process_header(FILE *c2_file, char *header_path, char *father_path, long depth)
{
    ENTRY entry;
    FILE *header_file = fopen(header_path, "r");
    char name_and_father[2 * FILE_NAME_LENGTH + 3];
    boolean_e ret = FALSE;

    entry.data = NULL;

    if (!header_file)
    {
        printf("error opening file: process_header\n");
    }
    else
    {
        ENTRY * temp;
        snprintf(name_and_father, sizeof(name_and_father), "%s//%s", header_path, father_path);
        printf("try %s\n", name_and_father);

        entry.key = strdup(name_and_father);

        temp = hsearch(entry, FIND); /*header_path//father_path */

        if(!temp)
        {
            entry.data = (void *)depth;
            hsearch(entry,ENTER);
            process_file(c2_file, header_file, header_path);
        }
        else if (temp->data != (void *)depth)
        {
            printf("storm detected!\n");
            ret = TRUE;
        }
        else
        {
            printf("depth %ld %s\n", depth, header_path);
            process_file(c2_file, header_file, header_path);
        }

        fclose(header_file);
    }

    return ret;
}

void process_file(FILE *c2_file, FILE *input, char *input_path)
{
    char line[LINE_LENGTH];
    char header_path[FILE_NAME_LENGTH];
    static int depth = 0;
    depth++;

    header_path[0] = '\0';

    while (fgets(line, LINE_LENGTH, input))
    {
        find_header(line, header_path);
        /* printf("header: %s\n", header_path); */
        if (*header_path)
        {
            process_header(c2_file, header_path, input_path, depth);
        }
        else
        {
            /* printf("%s", line); */
            fwrite(line, sizeof(char), strlen(line), c2_file);
        }
    }
    fputc('\n', c2_file);
    fflush(c2_file);

    depth--;
}

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

        c1_file = fopen(c1_file_name, "r");
        c2_file = fopen(c2_path, "w");

        if (!c1_file || !c2_file)
        {
            printf("error opening file in storm check\n");
        }

        else
        {
            process_file(c2_file, c1_file, c1_file_name);
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
