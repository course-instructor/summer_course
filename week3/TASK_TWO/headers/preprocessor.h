#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define FILE_NAME_LENGTH 256
#define LINE_LENGTH 256

#define HASH_SIZE 500
/**/
typedef enum {ERROR = -1, FALSE, TRUE } boolean_e;



boolean_e check_comment_state_change(char chr, FILE *file, boolean_e is_start);
char * remove_comments(char *c_file_name, boolean_e * success);

boolean_e preprocessor_check_for_include(char *line, char *ret);
void process_header(FILE *c2_file, char *header_path);
void process_file(FILE * file, FILE * head);
void preprocessor_run(char * c1_file_name);



#endif