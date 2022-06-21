#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <cutil/misc.h>

c_stack *new_c_stack(uint16_t c_size, uint16_t init_cap) {
    c_stack *s = safe_malloc(sizeof(c_stack));
    
    s->data = (uint8_t *)safe_malloc(c_size * init_cap);
   
    s->cell_size = c_size;
    s->top_ind = 0;
    s->cap = init_cap;

    return s; 
}

void *c_stack_push(c_stack *s) {
    if (s->top_ind == s->cap) {
        // Expansion is needed.
        uint8_t *new_data = safe_malloc((1 + s->cap * 2) * s->cell_size);
        
        // Copy old data into new data.
        memcpy(new_data, s->data, s->cap * s->cell_size); 

        free(s->data);

        s->data = new_data;
        s->cap *= 2;
    }

    return &(s->data[s->top_ind++ * s->cell_size]);
}


