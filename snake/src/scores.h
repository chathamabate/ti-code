#ifndef SCORES_H
#define SCORES_H

#include <stdint.h>

#define SCOREBOARD_SIZE 10
#define NAME_LEN 4

// 10 Highscores always...

typedef struct high_score_entry {
    char name[NAME_LEN];
    uint16_t score;
} hs_entry;

// Load the scoreboard from the archive.
// Returns NULL on failure.
hs_entry *load_sb();

// Returns 0 on failure.
// 1 on success.
uint8_t archive_sb(hs_entry *sb);

// Given a score, returns its corresponding index in the score table...
// should it be entered. Returns SCOREBOARD_SIZE if score is
// not higher than any scores on the board.
uint8_t sb_placement(hs_entry *sb, uint16_t score);

// Returns 1 if name is available.
// 0 otherwise.
uint8_t test_name(char name[NAME_LEN]);

// insert the new highscore into the score board at the given
// index.
void sb_insert(uint8_t index, hs_entry new_hs);

#endif