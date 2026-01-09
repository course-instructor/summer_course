#ifndef SET_H
#define SET_H

typedef unsigned int set[4];

int add_to_set(set set, int num);
void print_set(set set);
void union_set(set seta, set setb, set newset);
void intersect_set(set seta, set setb, set newset);
void sub_set(set seta, set setb, set newset);
void symdiff_set(set seta, set setb, set newset);
void stop();

#endif
