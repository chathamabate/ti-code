#include "./centered_pane.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/gui/scroll_text_pane.h"
#include "graphx.h"

#include <cxxutil/gui/centered_pane.h>

using namespace cxxutil::gui;

const scroll_text_pane_info_t STP_INFO = {
    .height = 65,
    .lineWidth = 83,
    .scrollBarWidth = 8,

    .bgColor = 40,
    .scrollBarBGColor = 194,
};

const centered_pane_info_t CP_INFO = {
    .titleWidthScale = 1,
    .titleHeightScale = 2,
    .titleFGColor = 175,
    .titleBGColor = 40,
    .titlePadding = 2,

    .borderBGColor = 23,

    .width = 200,
    .height = 160,
};

void cxxutil::gui::runCenteredPaneGUITest() {
    ScrollTextPane *stp = new ScrollTextPane(1, &STP_INFO); 
    CenteredPane<ScrollTextPane> *cp = 
        new CenteredPane<ScrollTextPane>(1, &CP_INFO, "Title", stp);

    gfx_Begin();
    gfx_SetDrawScreen();

    gfx_FillScreen(255);

    cp->render(20, 20);

    gfx_SwapDraw();
    gfx_Wait();

    core::waitClear();

    gfx_End();

    delete cp;
    delete stp;
}
