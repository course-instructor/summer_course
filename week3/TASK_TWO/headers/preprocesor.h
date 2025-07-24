#ifndef PREPROCESSOR_H

#define PREPROCESSOR_H

#define FILE_NAME_LENGTH 256
#define LINE_LENGTH 256

#define HASH_SIZE 500

typedef enum { FALSE, TRUE } boolean_e;

typedef struct
{
    char file_name [FILE_NAME_LENGTH];
    char fathers[][FILE_NAME_LENGTH];
}fathers_file_s;


boolean_e check_comment_start(char chr, FILE * file);
boolean_e check_comment_end(char chr, FILE *file);
void remove_comments( char *c_file_name);

char * find_header(char * line);
void check_storm(ENTRY * entry, char * file_path);
boolean_e hash_search(char * file_path);
void hash_add(fathers_file_s file);
void handle_call(FILE * c2_file,char * header_path, char * father_path);
void func(FILE * file, FILE * head, char * father_path);
void detect_loop(char * c1_file_name);




#endif