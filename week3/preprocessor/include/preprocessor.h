#define PREOROCESSOR_H
#ifdef PREOROCESSOR_H

#include <stdbool.h>
#include <search.h>
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#define MAX_FILE_PATH_LEN 255

/**
 * @brief start program
 * @param src_file_path path to source file to preprocess
 */
void preprocess         (char * file_path);

/**
 * @brief goes over file and all files that are included (recursively)
 * @param src_file_path path to source file to preprocess
 * @param dest_file_path path to new file (.c3) after preprocessing
 */
void includer           (char * src_file_path,char *dest_file_path);

/**
 * @brief get path to file from include line
 * @param line string that might have include file
 * @param new_file_path copy found file from include into this string
 * @param src_file_path for getting the folder of the the orignal filel
 * @return true if line contains include , otherwise retutn false
 */
bool get_file_included(char * line, char * new_file_path,char * src_file_path);


/**
 * @brief goes over a given file and remove all comments
 * @param read_file_path input file with comments
 * @param write_file_path output file without comments
 */
void remove_comments    (char * read_file_path, char * write_file_path);

/**
 * @brief copies string until a given char
 * @param src source string
 * @param dest destination string
 * @param stop copy string until this char
 * @param size size of source and destination string
 */
void copy_until_char    (const char *src, char *dest, char stop, size_t size) ;

/**
 * @brief goes over string until a non-space charcter
 * @param line string with spaces
 * @param index checking start position
 */
int skip_spaces         (char * line,int index);

void get_folder_path(const char *full_path, char *folder_path);

#endif