#ifndef PREPROCESSOR_H

#define PREPROCESSOR_H

#define FILE_NAME_LENGTH 256
#define LINE_LENGTH 256

typedef enum { FALSE, TRUE } boolean_e;

typedef struct
{
    char file_name [FILE_NAME_LENGTH];
    char fathers[][FILE_NAME_LENGTH];
}fathers_file_s;


boolean_e check_comment_start(char chr, FILE * file);
boolean_e check_comment_end(char chr, FILE *file);

void remove_comments( char *c_file_name);

#endif