#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void find_header(char * line, char * * ret)
{
    char chr;
    char file_name[LINE_LENGTH];

    sscanf(line, "#include \"%s.h\" %d", &file_name, &chr);
    if(chr == EOF || chr == '\n')
    {
        *ret = file_name;
    }
    else
    {
        *ret = NULL;
    }
    return (ret);
}

void handle_call(FILE * file,FILE * header)
{
    if(!in_hassh(header))
    {
        add_hash(header);
        func(file,header);
    }
}

FILE * handle_include(char * header_path)
{
    FILE * header = fopen(header_path, "r");
    if(header)
    {

    }
}

boolean_e func(FILE * file, FILE * head)
{
    char line [LINE_LENGTH];
    char header_path[LINE_LENGTH];

    while (fgets(line,LINE_LENGTH, &head))
    {
        find_header(line, &header_path);
        if(*header_path)
        {
            FILE * res = fopen(header_path, "r");
            if(res)
            {

                handle_call(file,res);
                fclose(res);
            }
        }
        else
        {
            write_line(line);
        }
    }

}

boolean_e detect_loop(char * file_name)
{
    char line [LINE_LENGTH];

    FILE * file = fopen(file_name,"r");

    fgets(line,LINE_LENGTH,&file);

}

int main()
{
    char * c_file_name = "src/preprocessor.c";
    remove_comments(c_file_name);
    return 0;
}
