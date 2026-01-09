#include <stdio.h>
#include "include/preprocessor.h"


int main(int argc, char * args[])
{
    if(argc == 1)
    {
        printf("No arguments specified\n");
    }
    else
    {
        hcreate(BUFSIZ);
        for (int i = 1; i < argc; i++) 
        {
            printf("%s\n",args[i]);
            preprocess(args[i]);

        }
    }
    return 0;
}