#include "cgraphx.h"
#include "graphx.h"
#include <tice.h>

static uint8_t border_size = 4;

void cgfx_set_pane_border_size(uint8_t b_size) {
    border_size = b_size;
}

void cgfx_pane_nc(const cgfx_pane_style *style, uint16_t x, uint8_t y, uint16_t width, uint8_t height) {
    uint8_t width_scale = (width - (2 * border_size)) / border_size;
    uint8_t height_scale = (height - (2 * border_size)) / border_size;

    gfx_sprite_t **borders = style->border_sprites;

    gfx_Sprite_NoClip(borders[P_NW], x, y);
    gfx_ScaledSprite_NoClip(borders[P_N], x + border_size, y, width_scale, 1);
    gfx_Sprite_NoClip(borders[P_NE], x + width - border_size, y);
    gfx_ScaledSprite_NoClip(borders[P_E], x + width - border_size, y + border_size, 1, height_scale);
    gfx_Sprite_NoClip(borders[P_SE], x + width - border_size, y + height - border_size);
    gfx_ScaledSprite_NoClip(borders[P_S], x + border_size, y + height - border_size, width_scale, 1);
    gfx_Sprite_NoClip(borders[P_SW], x, y + height - border_size);
    gfx_ScaledSprite_NoClip(borders[P_W], x, y + border_size, 1, height_scale);

    gfx_SetColor(style->fill);
    gfx_FillRectangle(x + border_size, y + border_size, width - (2 * border_size), height - (2 * border_size));
}

void cgfx_text_pane_nc(const text_pane_template *tmplt) {
    cgfx_pane_nc(tmplt->style, tmplt->x, tmplt->y, tmplt->w, tmplt->h);

    gfx_SetTextScale(tmplt->text_w_sc, tmplt->text_h_sc);
    gfx_SetTextFGColor(tmplt->text_color);
    gfx_PrintStringXY(
            tmplt->text, 
            tmplt->x + (tmplt->w - gfx_GetStringWidth(tmplt->text)) / 2,
            tmplt->y + (tmplt->h - 8 * tmplt->text_h_sc) / 2
    );
}
