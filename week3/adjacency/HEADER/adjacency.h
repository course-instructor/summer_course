#ifndef adjacency_h
#define adjacency_h

#define N 4 /* the size of row and col */
#define TRUE 1
#define FALSE 0

typedef struct
{
    int matrix[N][N];

}adjmat , *adjmat_ptr;

int find_path(adjmat_ptr adjmat, int u, int v);
int path(adjmat_ptr adjmat,int u , int v);
int init_matrix(adjmat_ptr adjmat);
int print_matrix(adjmat_ptr adjmat);
int check_init_mat(adjmat_ptr adjmat);
int check_eof(int flag_u);


#endif