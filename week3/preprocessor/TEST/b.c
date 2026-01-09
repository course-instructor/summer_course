#include <stdio.h>
#include <search.h>
/*MAIN FUNCTION*/
int main()
{
    ENTRY item;
    ENTRY * ret;
    hcreate(BUFSIZ);

    item.key = "hello";
    item.data = "WHAT";
    
    hsearch(item,ENTER);
    item.key = "yee";
    item.data = "WHO";
    hsearch(item,ENTER);
    item.key = "hello";
    hsearch(item,FIND);
    //printf("%s\n",ret->data);



    return 0;
}
