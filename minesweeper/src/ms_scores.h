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

#endif
