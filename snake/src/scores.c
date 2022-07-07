#include "scores.h"

#include <string.h>
#include <fileioc.h>
#include <graphx.h>
#include <string.h>

#define ERROR_OUT(msg) \
    sb_error_msg = msg; \
    return 0;

static const char *SB_FN = "SnkD";
static const char *TEMP_SB_FN = "SnkDT";

char *sb_error_msg = "N/A";

static uint8_t def_name_generated = 0;
static char def_name[SB_NAME_LEN + 1];

uint8_t load_sb(hs_entry *sb) {
    uint8_t index;

    // This will run the first time load is called.
    // The purpose is to set up the default name without needing an 
    // init function.
    if (!def_name_generated) {
        for (index = 0; index < SB_NAME_LEN; index++) {
            def_name[index] = '_';
        }
        def_name[SB_NAME_LEN] = '\0';

        def_name_generated = 1;
    }

    ti_var_t sb_data = ti_Open(SB_FN, "r");

    // Create the score board. DEPRECATED.
    // hs_entry *sb = malloc((sizeof(hs_entry) * SB_SIZE));

    if (sb_data == 0) {
        // If no scoreboard found, create a new blank one.
        for (index = 0; index < SB_SIZE; index++) {
            strcpy(sb[index].name, def_name);
            sb[index].score = 0;
        }

        return 1;
    }

    uint8_t scores_read = ti_Read(sb, sizeof(hs_entry), SB_SIZE, sb_data);

    if (!ti_Close(sb_data)) {
        sb_error_msg = "load_sb: Error closing SB!";
        return 0;
    }

    if (scores_read != SB_SIZE) {
        sb_error_msg = "load_sb: Error reading sb!";
        return 0;
    }

    return 1;
}


uint8_t archive_sb(hs_entry *sb) {
    ti_var_t temp_sb_data = ti_Open(TEMP_SB_FN, "w");

    uint8_t data_written = ti_Write(sb, sizeof(hs_entry), SB_SIZE, temp_sb_data);

    if (!ti_SetArchiveStatus(1, temp_sb_data)) {
        sb_error_msg = "archive_sb: Can't Archive SB!";

        // Even if archive fails, make sure to close file.
        if (!ti_Close(temp_sb_data)) {
            sb_error_msg = "archive_sb: Can't Archive SB! And can't Close SB!";
        }

        return 0;
    }

    if (!ti_Close(temp_sb_data)) {
        ERROR_OUT("archive_sb: Can't close SB!");
    }

    if (data_written != SB_SIZE) {
        ERROR_OUT("archive_sb: Can't write SB!");
    }

    // If we don't delete the file.
    // An error should occur in the rename phase... kinda a hack.
    ti_Delete(SB_FN); 

    uint8_t rename_status = ti_Rename(TEMP_SB_FN, SB_FN);

    if (rename_status == 1) {
        ERROR_OUT("archive_sb: Rename already exists!");
    }

    if (rename_status == 2) {
        ERROR_OUT("archive_sb: Unknown Rename error!");
    }

    return 1;
}

uint8_t sb_placement(hs_entry *sb, uint16_t score) {
    uint8_t index;
    for (index = SB_SIZE; index > 0 && sb[index - 1].score < score; index--);

    return index;
}

uint8_t name_available(hs_entry *sb, char name[SB_NAME_LEN + 1]) {
    uint8_t index;
    for (index = 0; index < SB_SIZE; index++) {
        if (strcmp(name, sb[index].name) == 0) {
            return 0;
        }
    }

    return 1;
}

void sb_insert(hs_entry *sb, uint8_t index, hs_entry *new_hs) {
    if (index >= SB_SIZE) {
        return;
    }

    uint8_t k;

    for (k = SB_SIZE - 1; k > index; k--) {
        sb[k] = sb[k - 1];
    }

    sb[index] = *new_hs;
}

void render_sb_xy(hs_entry* sb, uint16_t x, uint8_t y) {
    gfx_SetColor(COLOR_3);
    gfx_FillRectangle(x, y, SB_WIDTH, SB_HEIGHT);

    gfx_SetTextScale(SB_TXT_W, SB_TXT_H);
    gfx_SetTextFGColor(COLOR_0);

    uint8_t i;
    char score_line[SB_CHARS_PER_LINE + 1];
    score_line[SB_CHARS_PER_LINE] = '\0';

    for (i = 2 + SB_NAME_LEN + 1; i < 2 + SB_NAME_LEN + SB_SPACERS - 1; i++) {
        score_line[i] = SB_SPACER;
    }

    // This spaces present in the same place on every line.
    score_line[1] = ' ';
    score_line[2 + SB_NAME_LEN] = ' ';
    score_line[2 + SB_NAME_LEN + SB_SPACERS - 1] = ' ';

    uint16_t cur_x = x + SB_BORDER;
    uint8_t cur_y = y + SB_BORDER;

    for (i = 0; i < SB_SIZE; i++) {
        score_line[0] = '1' + i;

        uint8_t j;

        // Copy the name into the score line.
        for (j = 0; j < SB_NAME_LEN; j++) {
            score_line[2 + j] = sb[i].name[j];
        }

        char buff[5 + 1];   // 1 for the null terminator. 
        sprintf(buff, "%d", sb[i].score);
        uint8_t digits = dec_digits(sb[i].score);
        
        right_align(&score_line[2 + SB_NAME_LEN + SB_SPACERS], 5, buff, digits, ' ');

        render_string_xy_mono(score_line, SB_CHARS_PER_LINE, cur_x, cur_y, SB_TXT_W, SB_TXT_H);
        // gfx_PrintStringXY(score_line, cur_x, cur_y);

        cur_y += (8 * SB_TXT_H) + SB_BORDER;
    }
}



