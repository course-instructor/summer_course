#define PREOROCESSOR_H
#ifdef PREOROCESSOR_H

typedef enum bool_e
{
    FALSE = 0,
    TRUE   = 1
}bool;
void includer           (char * src_file_path,char *dest_file_path);
int skip_spaces         (char * line,int index);
void preprocess         (char * file_path);
void remove_comments    (char * read_file_path, char * write_file_path);
int get_file_included   (char * line, char * new_file_path);
void copy_until_char    (const char *src, char *dest, char stop, size_t size) ;


#endif