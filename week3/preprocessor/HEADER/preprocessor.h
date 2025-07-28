#ifndef preprocessor_h
#define preprocessor_h
#include <stdlib.h>
#include <search.h>
#include <libgen.h> 
#include <limits.h> 


enum {NOT_COMMENT, COMMENT};
enum {SUCCESS, FAILURE};

static int file_parent_table_size_g = 1;
#define MAX_FILE_NAME_LENGTH 256
#define MAX_LINE 1000

typedef struct
{
    char file_name[MAX_FILE_NAME_LENGTH];
    char *father;
}file_parent_record;

static file_parent_record *file_parent_table = NULL;

char * read_file(char *file_name);
char* write_to_file(char *file_name, char *text);
int init_file_parent_table();
int search_index(char *file_name);
int add_file_parent_record(char *file_name, char *father);
int search_index(char *file_name);
int add_file_parent_record(char *file_name, char *father);
void write_the_include(char *file_c1,char *output_file);



#endif