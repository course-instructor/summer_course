#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define FILE_NAME_LENGTH 256
#define LINE_LENGTH 256

#define HASH_SIZE 500
/**/
typedef enum { FALSE, TRUE } boolean_e;



boolean_e check_comment_start(char chr, FILE * file);
boolean_e check_comment_end(char chr, FILE *file);
void remove_comments( char *c_file_name);

void find_header(char * line, char * ret);
void process_header(FILE * c2_file,char * header_path, char * father_path, long depth);
void process_file(FILE * file, FILE * head, char * father_path);
void turn_c1_to_c2(char * c1_file_name);



#endif