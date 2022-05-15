
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <tice.h>
#include <string.h>

#include "misc.h"
#include "snake.h"
#include "scores.h"
#include "snake_render.h"

#define SB_ERROR \
    os_PutStrFull(sb_error_msg); \
    while (os_GetCSC() != sk_Clear); \
    return 0;

static void start(void);

int main(void) {
    coord c;
    c.x = 30;
    c.y = 30;

    button bts[4];
    bts[0].text = "Hey";
    bts[1].text = "Yo Yo Yo";
    bts[2].text = "Meh";
    bts[3].text = "Uh huh";

    size_t s = strlen("HEY");
    char buff[20];
    sprintf(buff, "STRLEN : %d", s);
    os_ClrHome();
    os_PutStrFull(buff);
    while (os_GetCSC() != sk_Clear);


    menu *m = new_menu(bts, 4);

    gfx_SetDrawBuffer();

    gfx_Begin();

    gfx_FillScreen(255);
    render_menu(m, c);

    gfx_SwapDraw();
    gfx_Wait();

    while (os_GetCSC() != sk_Clear);

    gfx_End();
    free(m);
    return 0;
}

