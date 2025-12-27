#pragma once
#include <stdint.h>

#define SET_ARRAY_COUNT 4
#define SET_ARRAY_LENGTH 32

typedef uint32_t set_t[SET_ARRAY_COUNT];

void init_set(set_t * set_ptr);
void add_item_to_set(set_t set, int num);
void print_set(const set_t set);
void union_set( set_t  first,  set_t  second, set_t result);
void intersect_set( set_t first, set_t second, set_t result);
void sub_set( set_t first,  set_t second, set_t result);
void symdiff_set( set_t first, set_t second, set_t result);
