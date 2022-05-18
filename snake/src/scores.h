#ifndef SCORES_H
#define SCORES_H

#include <stdint.h>
#include "misc.h"

#define SB_SIZE 8 // Must be < 10.
#define SB_NAME_LEN 4

// Number of character spacers between a name and 
// its score.
#define SB_SPACERS 8
#define SB_SPACER '.'

#define SB_TXT_W 2
#define SB_TXT_H 2

#define SB_BORDER 2

#define SB_CHARS_PER_LINE (2 + SB_NAME_LEN + SB_SPACERS + 5)

#define SB_WIDTH ((2 * SB_BORDER) + (SB_TXT_W * 8 * SB_CHARS_PER_LINE))
#define SB_HEIGHT (((SB_BORDER + (SB_TXT_H * 8)) * SB_SIZE) + SB_BORDER)

typedef struct high_score_entry {
    char name[SB_NAME_LEN + 1];
    uint16_t score;
} hs_entry;

// This will always store the last error message.
extern char *sb_error_msg;

// Load the scoreboard from the archive.
// If the scoreboard does not exist, return a new empty
// scoreboard.
// Returns NULL on failure.
hs_entry *load_sb();

// Returns 0 on failure.
// 1 on success.
uint8_t archive_sb(hs_entry *sb);

// Given a score, returns its corresponding index in the score table...
// should it be entered. Returns SB_SIZE if score is
// not higher than any scores on the board.
uint8_t sb_placement(hs_entry *sb, uint16_t score);

// Returns 1 if name is available.
// 0 otherwise.
uint8_t name_available(hs_entry *sb, char name[SB_NAME_LEN + 1]);

// insert the new highscore into the score board at the given
// index.
void sb_insert(hs_entry *sb, uint8_t index, hs_entry *new_hs);

#define render_sb(sb, c) render_sb_xy(sb, (c).x, (c).y)

// Render the scoreboard at a certain coordinate.
void render_sb_xy(hs_entry* sb, uint16_t x, uint8_t y);

#endif