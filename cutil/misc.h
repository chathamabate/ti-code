#ifndef MISC_H
#define MISC_H

#include <stdio.h>

typedef void (*malloc_fail_routine)(void);

extern malloc_fail_routine mf_routine;

void normal_malloc_fail_routine(void);
void graphics_malloc_fail_routine(void);

// This call works just like malloc.
// However, if the calculator is out of memory, the malloc_fail_routine 
// will be called.
void *safe_malloc(size_t size);

#endif