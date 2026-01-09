#ifndef SET_H
#define SET_H
#include <stdio.h>
typedef long long set[2];

void initialize_set(set* s);
void read_set(set** s,int * numbers);
void print_set(set ** s);
void union_set(set ** s);
void intersect_set(set ** s);
void sub_set(set **s);
void symdiff_set(set ** s);


#endif