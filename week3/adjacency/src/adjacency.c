#include "../header/adjacency.h"


/**
 * @brief Finds out if there existing path from node two nodes
 * @param tree Adjacency matrix
 * @param v Source node
 * @param u Destination node
 * @return TRUE if path exits or FALSE if it doesn't
 */
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
                return TRUE;
            }
            /*if not repeat the opertaion on the found node*/
            return path(tree,i,u);
            
        }
        
    }
    /*return false if node isnt pointing to any other node*/
    return FALSE;
    
}



