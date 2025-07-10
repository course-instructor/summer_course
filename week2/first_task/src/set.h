#pragma once
#include <stdint.h>

#define SET_SIZE 4

typedef uint32_t set_t[SET_SIZE];

void init_set(set_t * set_ptr);
void add_item_to_set(set_t set, int num);
void read_set(set_t set, const char *items);
void print_set(const set_t set);
void union_set( set_t  first,  set_t  second, set_t result);
void intersect_set( set_t first, set_t second, set_t result);
void sub_set( set_t first,  set_t second, set_t result);
void symdiff_set( set_t first, set_t second, set_t result);
