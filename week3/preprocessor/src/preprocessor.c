#define _GNU_SOURCE
#include <stdio.h>

#include "../include/preprocessor.h"



void preprocess(char * src_file_path)
{
    

    /*File without comments name*/
    char absolute_path[MAX_FILE_PATH_LEN];
    char new_file[MAX_FILE_PATH_LEN];
    
    realpath(src_file_path,absolute_path);

    includer(absolute_path,new_file);
    

}

void includer(char * src_file_path,char *dest_file_path)
{

    ENTRY found_item;
    

    char line[BUFSIZ];
    char file_name[MAX_FILE_PATH_LEN] = {0}; /*name of the file with .c */

    /*files*/
    char new_file_path[MAX_FILE_PATH_LEN] = {0};
    char file_path_no_comments[MAX_FILE_PATH_LEN] = {0};
    char file_path_included[MAX_FILE_PATH_LEN] = {0};

    FILE * current_file = NULL, *dest_file = NULL , *included_file = NULL;

    /*create file with no comments (c1)*/
    remove_comments(src_file_path,file_path_no_comments);

    /*create final with includes (c2)*/
    strncpy(dest_file_path,src_file_path,MAX_FILE_PATH_LEN);
    strncat(dest_file_path,"2",2);
    
    /*get file name (with .c) for saving in a hashmap*/
    copy_until_char(src_file_path,file_name,'.',MAX_FILE_PATH_LEN);
    found_item.key = strdup(file_name);

    if(src_file_path == NULL)                   /*path not specified*/
    {
        printf("Invalid path\n");
    }
    else if( hsearch(found_item,FIND)!= NULL)   /*file exists in a hashmap - alredy was opened before*/
    {
        fprintf(stderr,"Header storm detected\n");

    }
    else if((current_file = fopen(file_path_no_comments,"r")) == NULL) /*couldnt find a file*/
    {
        fprintf(stderr,"File with the name [%s] does not exists\n",file_path_no_comments);

    }
    else if((dest_file = fopen(dest_file_path,"w")) == NULL)           /*couldnt create a file*/
    {
        fprintf(stderr,"Couldnt create file with the name [%s]\n",dest_file_path);
    }
    else
    {
        if(hsearch(found_item,ENTER)==NULL)
        {
            fprintf(stderr,"Set overflow\n");
        }
        /* Go over a file and copy it into file c2  */
        while (fgets(line,BUFSIZ,current_file) != NULL)
        {  
            /*if line contains include to another file, copy included file and then continue*/
            if(get_file_included(line,new_file_path,src_file_path) == true)
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
            memset(file_path_included,0,MAX_FILE_PATH_LEN); /*reset file path of include*/
            
        }

    }
    /* close files */
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

bool get_file_included(char * line, char * new_file_path,char * src_file_path)
{
    bool status = true;
    char folder_path[MAX_FILE_PATH_LEN];
    
    get_folder_path(src_file_path,folder_path);
    

    int line_index = 0 , new_file_index = 0;

    char reltive_path[MAX_FILE_PATH_LEN] = {0};
    line_index = skip_spaces(line,line_index);
    
    if (line[line_index] == '#')
    {
        line_index++;
        
        line_index = skip_spaces(line,line_index);

        if(strncmp(line+line_index,"include",7) == 0)
        {
            line_index+=strlen("include");
            line_index = skip_spaces(line,line_index);

            if(line[line_index] == '\"')
            {
                line_index++;
                while(new_file_index < MAX_FILE_PATH_LEN && line[line_index] != '\0' && line[line_index] != '\"' )
                {
                    reltive_path[new_file_index] =  line[line_index];
                    line_index++;
                    new_file_index++;
                }
            }
            
        
        }
    }
    strncat(folder_path,reltive_path,MAX_FILE_PATH_LEN-1);
    strncpy(new_file_path,folder_path,MAX_FILE_PATH_LEN);
    if(*reltive_path == '\0')
    {
        status = false;
    }

    return status;

}



void remove_comments(char * read_file_path, char * write_file_path)
{
    char next = 0;
    bool is_comment = false;
    FILE * read_file;
    FILE * write_file;

    strncpy(write_file_path,read_file_path,MAX_FILE_PATH_LEN);
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
                is_comment = true;
            }
            else if(is_comment && next == '*' && (next = fgetc(read_file)) == '/')
            {
                is_comment = false;
            }
            else if(is_comment == false && next != EOF)
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
void get_folder_path(const char *full_path, char *folder_path) 
{
    const char *last_slash = strrchr(full_path, '/');
    size_t len;
    if (last_slash && (last_slash - full_path) < MAX_FILE_PATH_LEN) 
    {
        len = last_slash - full_path + 1;  
        strncpy(folder_path, full_path, len);
        folder_path[len] = '\0';  
    } 
    else 
    {
        folder_path[0] = '\0'; 
    }
}