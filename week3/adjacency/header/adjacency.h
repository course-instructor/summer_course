#include <stdio.h>
#include <stdint.h>
#ifndef ADJACENCY_H
#define ADJACENCY_H

#define N 4

typedef enum bool_e
{
    FALSE   = 0,
    TRUE    = 1
}bool;

typedef int adjmat[N][N];

int path(adjmat tree, uint32_t v , uint32_t u);

bool adjmat_validation(adjmat matrix);
void get_metrix_form_input(adjmat * metrix);
void adjmat_print(adjmat matrix, int rows, int cols);
int get_two_numbers(int *v, int *u) ;

void print_instructions(void);

#endif