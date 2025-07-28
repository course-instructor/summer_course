#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../HEADER/preprocessor.h"

/**
 * @brief this file puts the whole file content into an array of chars
 * 
 * @param file_name the file we reading from
 * @return char* the array that contains the content
 */
char * read_file(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL) 
    {
        perror("Error opening file\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_code = malloc(file_size + 1);
    if (file_code == NULL)
    {
        perror("Memory allocation failed\n");
        fclose(file);
        return NULL; 
    }
    size_t read_bytes = fread(file_code,1,file_size,file);
    file_code[read_bytes] = '\0';
    fclose(file);
    return file_code;
}

/**
 * @brief we write to filec1 the content without the comments
 * 
 * @param file_name the file we are writing into
 * @param text the text we read from the original file 
 * @return char* we return the file name
 */
char* write_to_file(char *file_name, char *text)
{
    int index_buffer = 0;
    printf("strlen(text) : %ld\n",strlen(text));
    char *buffer_code = malloc(strlen(text) + 1);
    char *name = malloc(strlen(file_name) + 2); 
    int is_comment = NOT_COMMENT;
    strcpy(name,file_name);
    strcat(name,"1");
    FILE *file_commentless = fopen(name, "w");
    if(file_commentless == NULL)
    {
        perror("Error writing to file\n");
        return NULL;
    }
    while(*text != '\0')
    {
        if(*text == '/' && *(text+1) == '*')
        {
            while(!(*text == '*' && *(text+1) == '/'))
            {
                text++;
            }
            text+=2;
        }
        *(buffer_code + index_buffer++) = *text++;
    }
    buffer_code[index_buffer] = '\0';
    fwrite(buffer_code,1,strlen(buffer_code),file_commentless);
    fclose(file_commentless);
    free(buffer_code);
    return name;
}

/**
 * @brief we initialize the  files' parents array
 * 
 * @return int size of the array of files
 */

int init_file_parent_table() 
{
    file_parent_table = malloc(sizeof(file_parent_record) * 1);
    int size = 1; 
    if (!file_parent_table) 
    {
        perror("malloc failed");
        exit(1);
    }
    strcpy(file_parent_table[0].file_name,"/home/alon/Desktop/summer_army_course/week3/preprocessor/test/a.c");
    file_parent_table[0].father = malloc(1);
    if (!file_parent_table[0].father)
    {
        perror("malloc failed");
        exit(1);
    }
    file_parent_table[0].father[0] = '\0'; 
    return size;
}

/**
 * @brief this function checks if we are in the array of files
 * if we are, than we check the father, if we are not we add us
 * 
 * @param file_name the file we are checking about
 * @return int index
 */

int search_index(char *file_name)
{
    if (file_name == NULL)
    {
        fprintf(stderr, "search_index received NULL file_name\n");
        return -1;
    }
    int index = 0;
    for (index = 0; index < file_parent_table_size_g; index++) 
    {
        if (file_parent_table[index].file_name[0] != '\0' && strcmp(file_parent_table[index].file_name, file_name) == 0) 
        {
            return index;
        }
    }
    return -1; // Not found
}
/**
 * @brief we add here the file to the array and check if there is a storm
 * 
 * @param file_name the file we are checking about 
 * @param father the file we came from
 * @return int storm/not storm
 */
int add_file_parent_record(char *file_name, char *father)
{
    int index = search_index(file_name);

    if (index == -1) 
    {
        file_parent_table = realloc(file_parent_table, sizeof(file_parent_record) * (file_parent_table_size_g + 1));
        if (!file_parent_table) 
        {
            perror("realloc failed");
            exit(1);
        }

        int new_index = file_parent_table_size_g; 
        file_parent_table_size_g++; 
        /* we initialize the file contnet */
        file_parent_table[new_index].file_name[0] = '\0';
        strncpy(file_parent_table[new_index].file_name, file_name, MAX_FILE_NAME_LENGTH - 1);
        file_parent_table[new_index].file_name[MAX_FILE_NAME_LENGTH - 1] = '\0'; 

        file_parent_table[new_index].father = malloc(strlen(father) + 1);
        if (!file_parent_table[new_index].father)
        {
            perror("malloc failed");
            exit(1);
        }
        strcpy(file_parent_table[new_index].father, father);
        return 1;
    }
    else
    {
        /* if we didnt get here by a father that already got us here (loop) */
        if (file_parent_table[index].father && strstr(file_parent_table[index].father, father) == NULL)
        {
            size_t new_len = strlen(file_parent_table[index].father) + strlen(father) + 1;
            file_parent_table[index].father = realloc(file_parent_table[index].father, new_len);
            if (!file_parent_table[index].father)
            {
                perror("realloc failed");
                exit(1);                
            }
            strcat(file_parent_table[index].father, father);
            return 1;
        }
        else
        {
            printf("Header storm detected\n");
            return 0;
        }
    }
}

/**
 * @brief this is the recurisve function that writes to the file
 * 
 * @param file_c1 the file without the comments
 */

void write_the_include(char *file_c1,char *output_file)
{
    int retval;
    char *line = malloc(MAX_LINE);
    if (!line)
    {
        perror("malloc failed for line");
        exit(1);
    }

    char *file_c2 = malloc(strlen(file_c1) + 2);
    if (!file_c2)
    {
        perror("malloc failed for file_c2");
        free(line);
        exit(1);
    }

    strcpy(file_c2, file_c1);
    strcat(file_c2, "2");
    strcpy(output_file , file_c2);
    FILE *file_of_include = fopen(file_c2, "w");
    if (file_of_include == NULL)
    {
        perror("Error opening file for writing");
        free(line);
        free(file_c2);
        return;
    }

    FILE *file_commentless = fopen(file_c1, "r");
    if (file_commentless == NULL)
    {
        perror("Error opening file for reading");
        fclose(file_of_include);
        free(line);
        free(file_c2);
        return;
    }

    while (fgets(line, MAX_LINE, file_commentless) != NULL)
    {
        /*if we see an include */
        if (strstr(line, "#include") != NULL)
        {
            char *start_include_name = strchr(line, '"');
            /* if it's include that we need to write */
            if (start_include_name) 
            {
                char *end_include_name = strchr(start_include_name + 1, '"');
                if (end_include_name) 
                {
                    int len = end_include_name - start_include_name - 1;
                    char *include_path = malloc(len + 1);
                    if (!include_path) 
                    {
                        perror("malloc failed for include_path");
                        fclose(file_of_include);
                        fclose(file_commentless);
                        free(line);
                        free(file_c2);
                        exit(1);
                    }

                    strncpy(include_path, start_include_name + 1, len);
                    include_path[len] = '\0';
                    ENTRY search_file;
                    search_file.key = include_path;
                    ENTRY *found_file = hsearch(search_file, FIND);
                    
                    /* means we already wrote it into the file */
                    if(found_file != NULL)
                    {
                      
                        printf("File %s already exists in the table.\n", include_path);
                        free(include_path);            
                    }
                    else
                    {
                        hsearch(search_file, ENTER);

                        char *c1_cpy = strdup(file_c1); 
                        char *parent_dir = dirname(c1_cpy);
                        char full_include_path[MAX_FILE_NAME_LENGTH];
                        /* the real absoulute path of the file */
                        snprintf(full_include_path, sizeof(full_include_path), "%s/%s", parent_dir, include_path);
                        retval = add_file_parent_record(full_include_path, file_c1);
                        if (retval)
                        {
                            char *contnent_file;
                            contnent_file = read_file(full_include_path);
                            char *sub_file_c1 = write_to_file(full_include_path, contnent_file);
                            char child_file[MAX_FILE_NAME_LENGTH];
                            write_the_include(sub_file_c1,child_file);

                            FILE *included_file = fopen(child_file, "r");
                            if (included_file)
                            {
                                char included_line[MAX_LINE];
                                while (fgets(included_line, MAX_LINE, included_file))
                                {
                                    fwrite(included_line,strlen(included_line),sizeof(char), file_of_include);
                                }
                                fclose(included_file);
                            }
                            else
                            {
                                perror("Error opening included file");
                            }
                        }

                        free(c1_cpy);
                        free(include_path);
                    }
                    
                }
            }
            /* this is just a include<> so we just copy*/
            else
            {

                fwrite(line,strlen(line),sizeof(char), file_of_include);
            }
        }
        else
        {
            //printf("Writing to [%s] with [%s]\n", file_c2, line);

            fwrite(line,strlen(line),sizeof(char), file_of_include);

        }
    }

    fclose(file_of_include);
    fclose(file_commentless);
    free(line);
    free(file_c2);
}



