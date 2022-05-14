#include "scores.h"

#include <string.h>
#include <fileioc.h>

#define ERROR_OUT(msg) \
    sb_error_msg = msg; \
    return 0;

char *sb_error_msg = "N/A";

static uint8_t def_name_generated = 0;
static char def_name[NAME_LEN + 1];

hs_entry *load_sb() {
    uint8_t index;

    // This will run the first time load is called.
    // The purpose is to set up the default name without needing an 
    // init function.
    if (!def_name_generated) {
        for (index = 0; index < NAME_LEN; index++) {
            def_name[index] = '-';
        }
        def_name[NAME_LEN] = '\0';

        def_name_generated = 1;
    }

    ti_var_t sb_data = ti_Open(SB_FN, "r");

    // Create the score board.
    hs_entry *sb = malloc((sizeof(hs_entry) * SB_SIZE));

    if (sb_data == 0) {
        // If no scoreboard found, create a new blank one.
        for (index = 0; index < SB_SIZE; index++) {
            strcpy(sb[index].name, def_name);
            sb[index].score = 0;
        }

        return sb;
    }

    uint8_t scores_read = ti_Read(sb, sizeof(hs_entry), SB_SIZE, sb_data);

    if (!ti_Close(sb_data)) {
        sb_error_msg = "load_sb: Error closing SB!";

        free(sb);
        return NULL;
    }

    if (scores_read != SB_SIZE) {
        sb_error_msg = "load_sb: Error reading sb!";

        free(sb);
        return NULL;
    }

    return sb;
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

    if (!ti_Delete(SB_FN)) {
        ERROR_OUT("archive_sb: Can't delete original SB!");
    }

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

uint8_t name_available(hs_entry *sb, char name[NAME_LEN + 1]) {
    uint8_t index;
    for (index = 0; index < SB_SIZE; index++) {
        if (strcmp(name, sb[index].name) == 0) {
            return 0;
        }
    }

    return 1;
}

void sb_insert(hs_entry *sb, uint8_t index, hs_entry new_hs) {
    if (index >= SB_SIZE) {
        return;
    }

    uint8_t k;

    for (k = SB_SIZE - 1; k > index; k--) {
        sb[k] = sb[k + 1];
    }

    sb[index] = new_hs;
}



