#ifndef MS_SCORES_H
#define MS_SCORES_H

#include <stdint.h>
#include "ms_mem_channels.h"
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

#define del_ms_scoreboard(sb) safe_free(MS_SCORES_CHANNEL, sb)

// This is the error message of the last error from 
// the below IO function calls.
extern const char *sb_err_msg;

#define view_sb_err_msg() \
    os_ClrHome(); \
    os_PutStrFull(sb_err_msg); \
    while (!os_GetCSC())

// Load scoreboard from archive if it is there
// otherwise create a new scoreboard.
// returns NULL on error.
ms_scoreboard *load_ms_scoreboard();

// Store a scoreboard into the archive.
// returns 0 on error. 1 otherwise.
uint8_t store_ms_scorebaord(ms_scoreboard *sb);

#endif
