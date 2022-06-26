#ifndef MS_SCORES_H
#define MS_SCORES_H

#include <stdint.h>
#include "minesweeper.h"

// Scores per difficulty.
#define MS_SCORES_LEN 3

typedef struct {
    // The scores.
    uint16_t scores[MS_NUM_DIFFS][MS_SCORES_LEN];
} ms_scoreboard;

// Create a new blank scoreboard.
// All scores will be initialized to MS_SCORES_LEN.
ms_scoreboard *new_ms_scoreboard();

// Insert a score into the scoreboard.
// Returns MS_SCORES_LEN if the score is too high to be added.
// Otherwise returns the index in the specific difficulty
// of the new high score.
uint8_t insert_score(ms_scoreboard *sb, uint8_t diff_ind, uint16_t score);

#endif
