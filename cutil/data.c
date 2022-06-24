#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <cutil/misc.h>

c_stack *new_c_stack(uint16_t c_size, uint16_t init_cap) {
    c_stack *s = safe_malloc(DATA_CHANNEL, sizeof(c_stack));
    
    s->data = (uint8_t *)safe_malloc(DATA_CHANNEL, c_size * init_cap);
   
    s->cell_size = c_size;
    s->top_ind = 0;
    s->cap = init_cap;

    return s; 
}

void *c_stack_push(c_stack *s) {
    if (s->top_ind == s->cap) {
        uint16_t new_cap = 1 + 2*s->cap;
   
        // Expansion is needed.
        uint8_t *new_data = safe_malloc(DATA_CHANNEL, new_cap * s->cell_size);
        
        // Copy old data into new data.
        memcpy(new_data, s->data, s->cap * s->cell_size); 

        safe_free(DATA_CHANNEL, s->data);

        s->data = new_data;
        s->cap  = new_cap;
    }

    return &(s->data[s->top_ind++ * s->cell_size]);
}


