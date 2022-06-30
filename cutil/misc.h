#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <stdint.h>

typedef void (*malloc_fail_routine)(void);

// MUST BE EVEN!
#define MALLOC_CHANNELS 24

// Standard Memory Channels.
#define TEST_CHANNEL 0
#define MENU_CHANNEL 1
#define DATA_CHANNEL 2
#define KEY_CHANNEL  3

// Save space for free util channels.

#define FREE_CHANNEL_START 8

#define FREE_CHANNEL(x) (x + FREE_CHANNEL_START)

void print_malloc_channels(void);

// extern malloc_fail_routine mf_routine;
void normal_malloc_fail_routine(void);
void graphics_malloc_fail_routine(void);

// This call works just like malloc.
// However, if the calculator is out of memory, the malloc_fail_routine 
// will be called.
void *safe_malloc(uint8_t channel, size_t size);

void safe_free(uint8_t channel, void *ptr);

// Set the malloc fail routine.
void set_malloc_fail_routine(malloc_fail_routine mfr);

// Shift Items from the left side of a buffer to the right.
void shift_right(char *buff, char fill_char, uint16_t buff_len, uint16_t data_len);

#endif
