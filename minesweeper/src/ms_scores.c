
#include "ms_scores.h"
#include "cutil/misc.h"
#include "fileioc.h"
#include "ms_mem_channels.h"

ms_scoreboard *new_ms_scoreboard() {
    ms_scoreboard *sb = safe_malloc(MS_SCORES_CHANNEL, sizeof(ms_scoreboard));

    uint8_t diff_i, score_i;
    for (diff_i = 0; diff_i < MS_NUM_DIFFS; diff_i++) {
        for (score_i = 0; score_i < MS_SCORES_LEN; score_i++) {
            sb->scores[diff_i][score_i] = MS_TIMEOUT + 1;
        }
    }

    return sb;
}

uint8_t insert_score(ms_scoreboard *sb, uint8_t diff_ind, uint16_t score) {
    uint8_t s_i = MS_SCORES_LEN;

    // First locate insertion index.
    for (; s_i > 0 && sb->scores[diff_ind][s_i - 1] > score; s_i--);
    
    if (s_i < MS_SCORES_LEN) {
        // New High Score!!
        // Shift scores down from s_i.
        uint8_t s_t_i;
        for (s_t_i = MS_SCORES_LEN - 1; s_t_i > s_i; s_t_i--) {
            sb->scores[diff_ind][s_t_i] = 
                sb->scores[diff_ind][s_t_i - 1];
        }

        // Insert Highscore itself.
        sb->scores[diff_ind][s_i] = score;
    }

    return s_i;
}

// Where the data is stored.
static const char *MS_SB = "MSSB";
const char *sb_err_msg;

ms_scoreboard *load_ms_scoreboard() {
    uint8_t handle = ti_Open(MS_SB, "r");  

    if (handle == 0) {
        // NOTE case where scoreboard does not
        // exist yet... create a new one.

        return new_ms_scoreboard();
    }
    
    ms_scoreboard *sb = safe_malloc(MS_SCORES_CHANNEL, sizeof(ms_scoreboard)); 

    uint8_t error_exit = 0;

    if (ti_Read(sb, sizeof(ms_scoreboard), 1, handle) != 1) {
        sb_err_msg = "load: could not read data";
        error_exit = 1;
    }

    if (ti_Close(handle) == 0) {
        sb_err_msg = "load: could not close data";
        error_exit = 1;
    }
    
    if (error_exit) {
        safe_free(MS_SCORES_CHANNEL, sb);
        return NULL;
    }

    return sb;
}

uint8_t store_ms_scorebaord(ms_scoreboard *sb) {
    uint8_t handle = ti_Open(MS_SB, "w");

    // Some sort of open error here...
    if (handle == 0) {
        sb_err_msg = "store: could not open data";
        return 0;
    }

    if (ti_Write(sb, sizeof(ms_scoreboard), 1, handle) != 1) {
        sb_err_msg = "store: could not write data";
        ti_Close(handle);
        return 0;
    }
    
    if (ti_SetArchiveStatus(1, handle) == 0) {
        sb_err_msg = "store: could not archive data";
        ti_Close(handle);
        return 0;
    }

    if (ti_Close(handle) == 0) {
        sb_err_msg = "store: could not close data";
        return 0;
    }

    return 1;
}
