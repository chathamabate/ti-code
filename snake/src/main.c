
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#include <stdio.h>
#include <stdint.h>

// Screen Dimmensions...
// LCD_WIDTH = 320
// LCD_HEIGHT = 240

#define BORDER_COLOR 100
#define FOOD_COLOR 200

void print_centered(const char *str);
void draw_borders(void);

const uint8_t CELL_SIZE = 8;

static char *err_message = NULL;

int main(void) {
    // sleep(100); FIGURE OUT WHY THIS DOESN'T WORK LATER...

    gfx_Begin();
    gfx_SetDrawBuffer();

    if (LCD_WIDTH % CELL_SIZE || LCD_HEIGHT % CELL_SIZE) {
        err_message = "Bad cell size!";
        goto HANDLE_ERROR;
    }

    uint16_t width = LCD_WIDTH / CELL_SIZE;
    uint16_t height = LCD_HEIGHT / CELL_SIZE;

    uint16_t game_width = width - 2;
    uint16_t game_height = height - 2;

    if (height < 3) {
        err_message = "Cell size too large!";
        goto HANDLE_ERROR;
    }

    srand(rtc_Time());

    // Game Logic Here!
    uint24_t score = 0;
    uint16_t food_x = rand() % game_width;
    uint16_t food_y = rand() % game_height;

    gfx_SetColor(BORDER_COLOR);
    draw_borders();

    gfx_SetColor(FOOD_COLOR);
    gfx_FillRectangle((food_x + 1) * CELL_SIZE, (food_y + 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE);

    gfx_SwapDraw();
    while (!kb_IsDown(kb_KeyClear)) kb_Scan();


    goto EXIT;

HANDLE_ERROR:
    gfx_FillScreen(255);

    gfx_SetColor(BORDER_COLOR);
    draw_borders();

    gfx_SetColor(0);
    print_centered(err_message ? err_message : "Error!");

    gfx_SwapDraw();

    while (!kb_IsDown(kb_KeyClear)) kb_Scan();

EXIT:
    gfx_End(); 
    return 0;
}


/* Prints a screen centered string */
void print_centered(const char *str) {
    gfx_PrintStringXY(str,
                      (LCD_WIDTH - gfx_GetStringWidth(str)) / 2,
                      (LCD_HEIGHT - 8) / 2);
}

void draw_borders(void) {
    gfx_FillRectangle(0, 0, LCD_WIDTH, CELL_SIZE);
    gfx_FillRectangle(0, LCD_HEIGHT - CELL_SIZE, LCD_WIDTH, CELL_SIZE);
    gfx_FillRectangle(0, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
    gfx_FillRectangle(LCD_WIDTH - CELL_SIZE, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
}