#include "../header/adjacency.h"

int path(adjmat tree, uint32_t v , uint32_t u)
{
    uint32_t i;
    /*go over a line starting with v*/
    for (i = 0; i < N; i++)
    {
        /*if node is pointing to another node*/
        if(tree[i][v] == 1)
        {
            /*if it equals to u return true*/
            if(i == u)
            {
                return true;
            }
            /*if not repeat the opertaion on the found node*/
            return path(tree,i,u);
            
        }
        
    }
    /*return false if node isnt pointing to any other node*/
    return false;
    
}



