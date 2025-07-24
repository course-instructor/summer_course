#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <unistd.h>
#include <limits.h>

#include <../headers/preprocesor.h>


boolean_e check_comment_start(char chr, FILE * file)
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

            if(fseek(file, -1, SEEK_CUR))
            {
                perror("Error on fseek in check_comment_start");
            }
        }
    }
    return ret;
}


boolean_e check_comment_end(char chr, FILE * file)
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
            if(fseek(file, -1, SEEK_CUR))
            {
                perror("Error on fseek in check_comment_end");
            }
        }
    }
    return ret;
}


void remove_comments( char * c_file_name)
{
    char * c1_file_name = malloc(strlen(c_file_name) + 1);
    FILE * c_file;
    FILE * c1_file;

    char chr;
    boolean_e in_comment = FALSE;



    strcpy(c1_file_name,c_file_name);
    strcat(c1_file_name, "1");

    c_file = fopen(c_file_name, "r");
    c1_file = fopen(c1_file_name, "w");

    if(!c_file)
    {
        perror("Error opening c_file\n");

    }

    else if(!c1_file)
    {
        perror("Error opening c1_file\n");
    }

    else
    {
        while ((chr = fgetc(c_file)) != EOF)
        {
            if(!in_comment)
            {
                if(check_comment_start(chr, c_file))
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
                if(check_comment_end(chr, c_file))
                {
                    in_comment = FALSE;
                }
            }
        }
    }
    fclose(c_file);
    fclose(c1_file);
}


char * find_header(char * line)
{
    int chr;
    char file_name[LINE_LENGTH];

    sscanf(line, "#include \"%[^\"]\" %d", file_name, chr);
    if(chr == EOF || chr == '\n')
    {
        realpath(file_name,file_name);
    }
    else
    {
        *file_name = NULL;
    }
    return(file_name);
}

void check_storm(ENTRY * entry, char * file_path)
{

    if(strstr((char *)entry->data,file_path))
    {
        printf("storm detected!\n");
    }
    else
    {
        entry->data = realloc(entry->data, sizeof(entry->data ) + FILE_NAME_LENGTH);
        strcat((char *) entry->data, file_path);
    }
}

boolean_e hash_search(char * file_path)
{
    ENTRY temp;
    boolean_e ret = FALSE;

    temp.key = file_path;

    if(hsearch(temp, FIND))
    {
        ret = TRUE;
    }

    return ret;

}


void hash_add(fathers_file_s file)
{
    ENTRY temp;
    boolean_e ret = FALSE;

    temp.key = file.file_name;
    temp.data = file.fathers;

    hsearch(temp, ENTER);
}

void handle_call(FILE * c2_file,char * header_path, char * father_path)
{
    ENTRY * entry;
    FILE * header_file = fopen(header_path, "r");

    entry->key = header_file;
    entry->data = NULL;

    if(!header_file)
    {
        printf("error opening file: handle_call\n");
    }
    else
    {
        strncpy(entry->key, father_path, LINE_LENGTH);
        check_storm(entry, father_path);

        if(entry = hsearch(*entry,FIND))
        {
            strncpy(entry->data, header_path, LINE_LENGTH);


            hsearch(*entry, ENTER);

            func(c2_file, header_file,header_path);
        }
        else
        {

            strncpy((char*)entry->data,"\0",sizeof(char));
            strncat((char *)entry->data, father_path, LINE_LENGTH);
            hsearch(*entry, ENTER);
        }
    }
}



void func(FILE * file, FILE * head, char * father_path)
{
    char line [LINE_LENGTH];
    char header_path[FILE_NAME_LENGTH];

    while (fgets(line,LINE_LENGTH, head))
    {
        strncpy(header_path, find_header(line), FILE_NAME_LENGTH);
        if(*header_path)
        {
            FILE * res = fopen(header_path, "r");
            if(res)
            {

                handle_call(file,res,father_path);
                fclose(res);
            }
        }
        else
        {
            write_line(line);
        }
    }

}

void turn_c1_to_c2(char * c1_file_name)
{
    char line [LINE_LENGTH];
    char  c2_path[LINE_LENGTH];
    FILE * c2_file;
    FILE * c1_file;



    if(!hcreate(HASH_SIZE))
    {
        printf("error creating hash");
    }
    else
    {
        strncpy(c2_path, c1_file_name, LINE_LENGTH);
        c1_file = fopen(c1_file_name,"r");
        c2_file = fopen(c2_path,"w");

        if(!c1_file || !c2_file)
        {
            printf("error opening file in storm check\n");
        }

        else
        {
            func(c2_file,c1_file, NULL);
        }

        hdestroy();
    }

}

int main()
{
    char * c_file_name = "src/preprocessor.c";
    remove_comments(c_file_name);
    return 0;
}
