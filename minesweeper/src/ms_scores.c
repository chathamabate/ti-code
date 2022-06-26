
#include "ms_scores.h"
#include "cutil/misc.h"
#include "ms_mem_channels.h"

ms_scoreboard *new_ms_scoreboard() {
    ms_scoreboard *sb = safe_malloc(MS_SCORES_CHANNEL, sizeof(ms_scoreboard));

    uint8_t diff_i, score_i;
    for (diff_i = 0; diff_i < MS_NUM_DIFFS; diff_i++) {
        for (score_i = 0; score_i < MS_SCORES_LEN; score_i++) {
            sb->scores[diff_i][score_i] = MS_SCORES_LEN;
        }
    }
}

uint8_t insert_score(ms_scoreboard *sb, uint8_t diff_ind, uint16_t score) {
    uint8_t s_i = MS_SCORES_LEN;

    // First locate insertion index.
    for (; s_i > 0 && sb->scores[diff_ind][s_i - 1] > score; s_i--);
    
    if (s_i < MS_SCORES_LEN) {
        // New High Score!!
        // Shift scores down from s_i.
        uint8_t s_t_i;
        for (s_t_i = s_i + 1; s_t_i < MS_SCORES_LEN; s_t_i++) {
            sb->scores[diff_ind][s_t_i] = 
                sb->scores[diff_ind][s_t_i - 1];
        }
    }

    return s_i;
}

