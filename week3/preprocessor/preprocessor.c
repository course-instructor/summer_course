#define _GNU_SOURCE
#include <stdio.h>
#include <search.h>
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#define PATH "/home/asik/Documents/ARMY/summer_course/week3/preprocessor/TEST/"
#include "preprocessor.h"
#define MAX_FILE_PATH 255

ENTRY item;
ENTRY *ret;

int main(int argc, char * args[])
{
    hcreate(BUFSIZ);
    for (int i = 1; i < argc; i++) 
    {
        preprocess(args[i]);

    }

    return 0;
}

/**
 * @brief start program
 * @param src_file_path path to source file to preprocess
 */
void preprocess(char * src_file_path)
{
    

    /*File without comments name*/
    char absolute_path[MAX_FILE_PATH];
    char new_file[MAX_FILE_PATH];
    
    realpath(src_file_path,absolute_path);

    includer(absolute_path,new_file);
    

}
/**
 * @brief goes over file and all files that are included (recursively)
 * @param src_file_path path to source file to preprocess
 */
void includer(char * src_file_path,char *dest_file_path)
{

    ENTRY found_item;
    

    char line[BUFSIZ];
    char file_name[MAX_FILE_PATH] = {0}; /*name of the file with .c */

    /*files*/
    char new_file_path[MAX_FILE_PATH] = {0};
    char file_path_no_comments[MAX_FILE_PATH] = {0};
    char file_path_included[MAX_FILE_PATH] = {0};

    FILE * current_file = NULL, *dest_file = NULL , *included_file = NULL;

    /*create file with no comments (c1)*/
    remove_comments(src_file_path,file_path_no_comments);

    /*create final with includes (c2)*/
    strncpy(dest_file_path,src_file_path,MAX_FILE_PATH);
    strncat(dest_file_path,"2",2);
    
    /*get file name (with .c) for saving in a hashmap*/
    copy_until_char(src_file_path,file_name,'.',MAX_FILE_PATH);
    found_item.key = strdup(file_name);


    if(src_file_path == NULL)
    {
        printf("Invalid path\n");
    }
    else if( hsearch(found_item,FIND)!= NULL)
    {
        fprintf(stderr,"Header storm detected\n");

    }
    else if((current_file = fopen(file_path_no_comments,"r")) == NULL)
    {
        fprintf(stderr,"File with the name [%s] does not exists\n",file_path_no_comments);

    }
    else if((dest_file = fopen(dest_file_path,"w")) == NULL)
    {
        fprintf(stderr,"Couldnt create file with the name [%s]\n",dest_file_path);
    }
    else
    {
        if(hsearch(found_item,ENTER)==NULL)
        {
            fprintf(stderr,"Set overflow\n");
        }
        /* Go over a file and copy int file c2 , if line contains include to another file, copy included file and then continue*/
        while (fgets(line,BUFSIZ,current_file) != NULL)
        {  
            if(get_file_included(line,new_file_path) == TRUE)
            {   
                /*recusive call on found include*/
                includer(new_file_path,file_path_included);
                if((included_file = fopen(file_path_included,"r")) == NULL)
                {
                    fprintf(stderr,"File with the name [%s] does not exists\n",src_file_path);
                }
                else
                {
                    /*copy included file insted of the #include */
                    while(fgets(line,BUFSIZ,included_file) != NULL)
                    {
                        fputs(line,dest_file);
                    }
                    fclose(included_file);
                }

            }
            else     /* copy if no #include*/

            {
                fputs(line,dest_file);
            }
            memset(file_path_included,0,MAX_FILE_PATH);
            
        }

    }
    if(current_file != NULL)
    {
        fclose(current_file);

    }
    if(dest_file != NULL)
    {
        fclose(dest_file);
    }

    return;
}
/**
 * @brief get path to file from include line
 * @param line string that might have include file
 * @param new_file_path copy found file from include into this string
 * @return TRUE if line contains include , otherwise retutn FALSE
 */
int get_file_included(char * line, char * new_file_path)
{
    bool status = TRUE;
    char folder_path[MAX_FILE_PATH] = PATH;
    int line_index = 0 , new_file_index = 0;

    char reltive_path[MAX_FILE_PATH] = {0};
    line_index = skip_spaces(line,line_index);
    
    if (line[line_index] == '#')
    {
        line_index++;
        
        line_index = skip_spaces(line,line_index);

        if(strncmp(line+line_index,"include",7) == 0)
        {
            line_index+=7;
            line_index = skip_spaces(line,line_index);

            if(line[line_index] == '\"')
            {
                line_index++;
                while(new_file_index < MAX_FILE_PATH && line[line_index] != '\0' && line[line_index] != '\"' )
                {
                    reltive_path[new_file_index] =  line[line_index];
                    line_index++;
                    new_file_index++;
                }
            }
            
        
        }
    }
    strncat(folder_path,reltive_path,MAX_FILE_PATH-1);
    strncpy(new_file_path,folder_path,MAX_FILE_PATH);
    if(*reltive_path == '\0')
    {
        status = FALSE;
    }

    return status;

}



/**
 * @brief goes over a given file and remove all comments
 * @param read_file_path input file with comments
 * @param write_file_path output file without comments
 * 
 */
void remove_comments(char * read_file_path, char * write_file_path)
{
    char next = 0;
    bool is_comment = FALSE;
    FILE * read_file;
    FILE * write_file;

    strncpy(write_file_path,read_file_path,MAX_FILE_PATH);
    strncat(write_file_path,"1",2);

    if((read_file = fopen(read_file_path,"r")) == NULL)
    {
        fprintf(stderr,"File [%s] for reading does not exists\n",read_file_path);

    }
    else if((write_file = fopen(write_file_path,"w")) == NULL)
    {
        fprintf(stderr,"Couldnt create file for writing\n");
    }
    else
    {
        while((next = fgetc(read_file)) != EOF)
        {
            if(next == '/' && (next = fgetc(read_file)) == '*')
            {
                is_comment = TRUE;
            }
            else if(is_comment && next == '*' && (next = fgetc(read_file)) == '/')
            {
                is_comment = FALSE;
            }
            else if(is_comment == FALSE && next != EOF)
            {
                fwrite(&next,sizeof(char),1,write_file);
            }       
            
        }
    }
    if(read_file != NULL)
    {
        fclose(read_file);

    }
    if(write_file != NULL) 
    {
        fclose(write_file);
    }


}
/**
 * @brief copies string until a given char
 * @param src source string
 * @param dest destination string
 * @param stop copy string until this char
 * @param size size of source and destination string
 */
void copy_until_char(const char *src, char *dest, char stop, size_t size) 
{
    size_t i = 0;
    while (i + 1 < size && src[i] && src[i] != stop) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
int skip_spaces(char * line,int index)
{
    while(line[index] != '\0' && isspace(line[index]))
    {
        index++;
    }
    return index;

}
