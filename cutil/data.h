#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdlib.h>
#include <cutil/misc.h>

typedef struct {
    uint8_t *data;

    uint16_t cell_size;
    uint16_t top_ind; // Index of next free cell in data.
    uint16_t cap; // Number of cells all of data can hold.
} c_stack;

// Create a new stack with a given intial
// capcity.
c_stack *new_c_stack(uint16_t c_size, uint16_t init_cap);

// Pushes a new cell onto the stack, returns a pointer to
// the cell. (Will expand stack if needed)
void *c_stack_push(c_stack *s);

// NOTE for pop and peek, stack cannot be empty!
#define c_stack_peek(s) \
    (&((s)->data[((s)->top_ind - 1) * (s)->cell_size]))

#define c_stack_pop(s) \
    (s)->top_ind--;

#define c_stack_empty(s) \
    ((s)->top_ind == 0)

#define del_c_stack(s) \
    safe_free(DATA_CHANNEL, (s)->data); \
    safe_free(DATA_CHANNEL, s);

#endif
