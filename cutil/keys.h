#ifndef KEYS_H
#define KEYS_H

#include <keypadc.h>
#include <stdint.h>

// Keypad driver which handles key repetition.

// custum key type.
typedef uint8_t c_key_t;

/* Keyboard group 1 */
#define c_Graph    0
#define c_Trace    1
#define c_Zoom     2
#define c_Window   3
#define c_Yequ     4
#define c_2nd      5
#define c_Mode     6
#define c_Del      7

/* Keyboard group 1 - 83 Premium CE key names */
#define c_Fenetre  c_Window
#define c_Fx       c_Yequ
#define c_Suppr    c_Del


/* Keyboard group 2 */
#define c_Sto     8 
#define c_Ln      9 
#define c_Log     10 
#define c_Square  11 
#define c_Recip   12 
#define c_Math    13 
#define c_Alpha   14 

/* Keyboard group 2 - 83 Premium CE key names */
#define c_TglExact c_Recip

/* Keyboard group 3 */
#define c_0        15 
#define c_1        16 
#define c_4        17 
#define c_7        18 
#define c_Comma    19 
#define c_Sin      20 
#define c_Apps     21 
#define c_GraphVar 22 

/* Keyboard group 3 - 83 Premium CE key names */
#define c_Trig     c_Sin
#define c_Matrice  c_Apps

/* Keyboard group 4 */
#define c_DecPnt  23 
#define c_2       24 
#define c_5       25 
#define c_8       26 
#define c_LParen  27 
#define c_Cos     28 
#define c_Prgm    29 
#define c_Stat    30 

/* Keyboard group 4 - 83 Premium CE key names */
#define c_Resol    c_Cos

/* Keyboard group 5 */
#define c_Chs     31 
#define c_3       32 
#define c_6       33 
#define c_9       34 
#define c_RParen  35 
#define c_Tan     36 
#define c_Vars    37 

/* Keyboard group 5 - 83 Premium CE key names */
#define c_Frac     c_Tan


/* Keyboard group 6 */
#define c_Enter   38 
#define c_Add     39 
#define c_Sub     40 
#define c_Mul     41 
#define c_Div     42 
#define c_Power   43 
#define c_Clear   44 

/* Keyboard group 5 - 83 Premium CE key names */
#define c_Annul    c_Clear

/* Keyboard group 7 */
#define c_Down    45 
#define c_Left    46 
#define c_Right   47 
#define c_Up      48 

#define c_NumKeys c_Up + 1

// Map from c_key_t types to kb_lkey_t types.
extern kb_lkey_t c_key_map[c_NumKeys];

// Sets the keys which will be checked during a scan.
// The contents of keys will be copied into the library's global variables.
void set_focused_keys(const c_key_t *keys, uint8_t num_keys);

// Uses kb_Scan().
void scan_focused_keys(void);

typedef struct {
    c_key_t key;
    uint8_t count;
} focused_key;

// Do NOT use these manually, use macros instead.
extern focused_key *focused_keys;
extern uint8_t key_map[c_NumKeys];
extern uint8_t repeat_delay;

// This sets the number of scans to wait before repeat sets in.
#define set_repeat_delay(del) repeat_delay = del;

// This determines if a key should be considered pressed.
// Either the key was just pressed, or its been held for at least repeat delay scans.
#define key_press(c_key) \
    (focused_keys[key_map[c_key]].count == 1 || focused_keys[key_map[c_key]].count == repeat_delay)

#define key_down(c_key) \
    (focused_keys[key_map[c_key]].count >= 1)

// Convert a c_key_t to a kb_lkey_t.
#define to_kb_lkey_t(c_key) (c_key_map[c_key])

#endif
