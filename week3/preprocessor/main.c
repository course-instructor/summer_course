#include "./HEADER/preprocessor.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    hcreate(100);
    file_parent_table_size_g = init_file_parent_table();
    char *file_path = "/home/alon/Desktop/summer_army_course/week3/preprocessor/test/a.c";

    char *content_read = read_file(file_path);
    char *file_c1 = write_to_file(file_path, content_read);
    char file_c2[MAX_FILE_NAME_LENGTH];
    write_the_include(file_c1,file_c2);
    free(content_read);
    free(file_c1);
    free(file_parent_table);
    return 0;
}