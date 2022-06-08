
#include "misc.h"

#include <stdlib.h>

#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

void normal_malloc_fail_routine(void) {
    os_PutStrFull("Out of memory!");

    while (!kb_IsDown(kb_KeyClear)) {
        delay(50);
        kb_Scan();
    }

    exit(1);
}

void graphics_malloc_fail_routine(void) {
    gfx_End();
    normal_malloc_fail_routine();
}

static malloc_fail_routine mf_routine = normal_malloc_fail_routine;

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);

    if (ptr) {
        return ptr;
    }

    mf_routine();

    return NULL;
}

void set_malloc_fail_routine(malloc_fail_routine mfr) {
    mf_routine = mfr;
}
