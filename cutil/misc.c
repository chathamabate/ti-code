
#include "misc.h"
#include "ti/screen.h"

#include <stdlib.h>
#include <stdint.h>

#include <graphx.h>
#include <tice.h>
#include <keypadc.h>
#include <stdio.h>

static uint8_t malloc_counts[MALLOC_CHANNELS];

#define CHANNELS_PER_ROW 3

void print_malloc_channels(void) {
    char buff[20];
    uint8_t c;
    for (c = 0; c < MALLOC_CHANNELS; c++) {
        sprintf(buff, "%d: %d", c, malloc_counts[c]);
        os_PutStrFull(buff);

        if ((c + 1) % CHANNELS_PER_ROW == 0) {
            os_NewLine();
        } else {
            os_PutStrFull("  ");
        }
    } 
}

void normal_malloc_fail_routine(void) {
    os_ClrHome();
    os_PutStrFull("Out of memory!");
    os_NewLine();
    print_malloc_channels();

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

void *safe_malloc(uint8_t channel, size_t size) {
    if (size == 0) {
        return NULL;
    }

    void *ptr = malloc(size);

    if (ptr) {
        malloc_counts[channel]++;
        return ptr;
    }

    mf_routine();

    return NULL;
}

void safe_free(uint8_t channel, void *ptr) {
    free(ptr);

    if (ptr) {
        malloc_counts[channel]--;
    } 
}

void set_malloc_fail_routine(malloc_fail_routine mfr) {
    mf_routine = mfr;
}

void shift_right(char *buff, char fill_char, uint16_t buff_len, uint16_t data_len) {
    if (buff_len == data_len) {
        return; 
    }

    int24_t di, bi;
    for (di = data_len - 1, bi = buff_len - 1; di >= 0; di--, bi--) {
        buff[bi] = buff[di];
    }

    for (; bi >= 0; bi--) {
        buff[bi] = fill_char;
    }
}
