#ifndef SCORES_H
#define SCORES_H

#include <stdint.h>

#define SB_SIZE 10
#define NAME_LEN 4

// 10 Highscores always...

#define SB_FN "SnakeScores"
#define TEMP_SB_FN "SnakeScoresTemp"

typedef struct high_score_entry {
    char name[NAME_LEN + 1];
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
uint8_t name_available(hs_entry *sb, char name[NAME_LEN + 1]);

// insert the new highscore into the score board at the given
// index.
void sb_insert(hs_entry *sb, uint8_t index, hs_entry new_hs);

#endif