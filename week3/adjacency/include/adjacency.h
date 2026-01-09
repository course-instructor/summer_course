#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifndef ADJACENCY_H
#define ADJACENCY_H

#define N 4



typedef int adjmat[N][N];
/**
 * @brief Finds out if there existing path from node two nodes
 * @param tree Adjacency matrix
 * @param v Source node
 * @param u Destination node
 * @return TRUE if path exits or FALSE if it doesn't
 */
int path(adjmat tree, uint32_t v , uint32_t u);

/**
 * @brief prints welcome screen for user
 */
void print_instructions(void);
/**
 * @brief gets input from user of two numbers between 1 and N
 * @param v source node
 * @param u dest node
 */
bool get_two_numbers(int *v, int *u) ;
/**
 * @brief checks if adjmat metrix is legal - if it has only one head and no more then one node pointing to another node
 * @param metrix adjmat metrix
 */
bool adjmat_validation(adjmat matrix);
/**
 * @brief get metrix from user input, user may enter any charcters , but only 1 or 0 would be taken into account
 * @param metrix pointer adjmat metrix
 */
void get_metrix_form_input(adjmat * metrix);
/**
 * @brief prints metrix
 * @param row amont rows to print
 * @param cols amont of columns to print
 */
void adjmat_print(adjmat matrix, int rows, int cols);


#endif