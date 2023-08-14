#include "./scroll_text_pane.h"

#include "cxxutil/core/mem.h"
#include "cxxutil/gui/pane.h"
#include "ti/screen.h"

#include <cctype>
#include <cstdio>
#include <graphx.h>
#include <string.h>

using namespace cxxutil::gui;

ScrollTextPane::ScrollTextPane(uint8_t memChnl, const scroll_text_pane_info_t *stpi) 
    : Pane(memChnl), paneInfo(stpi) {
    this->blocks = new core::CoreList<const TextBlock *>(memChnl);

    this->viewInd = {
        .blockInd = 0,
        .lineInd = 0,
    };

    this->top = true;
    this->totalHeight = 0;
}

ScrollTextPane::ScrollTextPane(const scroll_text_pane_info_t *stpi) 
    : ScrollTextPane(core::CXX_DEF_CHNL, stpi) {
}

ScrollTextPane::~ScrollTextPane() {
    size_t blocksLen = this->blocks->getLen();
    for (size_t i = 0; i < blocksLen; i++) {
        delete this->blocks->get(i);
    }

    delete this->blocks;
}

void ScrollTextPane::render(uint24_t x, uint8_t y) {
    const scroll_text_pane_info_t *pi = this->paneInfo;

    gfx_SetColor(pi->bgColor);
    gfx_FillRectangle(x, y, pi->lineWidth, pi->height);

    gfx_SetColor(pi->scrollBarBGColor);
    gfx_FillRectangle(x + pi->lineWidth, y, pi->scrollBarWidth, pi->height);

    // Now we render each message one at a time...
    
}

bool ScrollTextPane::nextUp(tp_index_t i, tp_index_t *d) const {
    if (i.lineInd > 0) {
        d->blockInd = i.blockInd;
        d->lineInd = i.lineInd - 1;

        return true;
    }

    if (i.blockInd > 0) {
        size_t newBlockInd = i.blockInd - 1;

        d->blockInd = newBlockInd;
        d->lineInd = 
            this->blocks->get(newBlockInd)->getLines()->getLen() - 1;
    }

    return false;
}

bool ScrollTextPane::nextDown(tp_index_t i, tp_index_t *d) const {
    if (i.lineInd < this->blocks->get(i.blockInd)->getLines()->getLen() - 1) {
        d->blockInd = i.blockInd;
        d->lineInd = i.lineInd + 1;

        return true;
    }

    if (i.blockInd < this->blocks->getLen() - 1) {
        d->blockInd = i.blockInd + 1;
        d->lineInd = 0;

        return true;
    }

    return false;
}

void ScrollTextPane::scrollUp() {
    if (this->blocks->getLen() == 0) {
        return; // No lines.. do nothing!
    }

    if (this->top) {
        this->nextUp(this->viewInd, &(this->viewInd));
        return;
    }

    // NOTE: if !top, there must be a line to scroll up to.
    // Here, just find the first line which is not entirely visible.
    uint8_t aH = this->getLineHeight(this->viewInd);
    tp_index_t iter = this->viewInd;
    
    do {
        this->nextUp(iter, &iter);

        aH += this->paneInfo->vertLineSpace + 
            this->getLineHeight(iter);

    } while (aH <= this->paneInfo->height);

    this->top = true;
    this->viewInd = iter;
}

void ScrollTextPane::scrollDown() {
    if (this->blocks->getLen() == 0) {
        return; // No lines.. do nothing!
    }

    if (!(this->top)) {
        // Might do something, might not.
        this->nextDown(this->viewInd, &(this->viewInd));
        return;
    }

    // If top, we can only "scrollDown" if there exists a line below
    // viewInd which is not entirely visible. 
    //
    // Such a line might not always exist.

    uint8_t aH = this->getLineHeight(this->viewInd);
    tp_index_t iter = this->viewInd;

    do {
        if (!(this->nextDown(iter, &iter))) {
            return; // No line to scroll down to!
        }

        aH += this->paneInfo->vertLineSpace + 
            this->getLineHeight(iter);
    } while (aH <= this->paneInfo->height);

    this->top = false;
    this->viewInd = iter;
}

void ScrollTextPane::gotoTop() {
    this->viewInd = {
        .blockInd = 0,
        .lineInd = 0,
    };

    this->top = true;
}

void ScrollTextPane::gotoBottom() {
    if (this->totalHeight <= this->paneInfo->height) {
        return; // We must already be at the bottom.
                // And... top cannot equal false.
    }

    size_t lastBlockInd = this->blocks->getLen() - 1;

    this->viewInd = {
        .blockInd = lastBlockInd,
        .lineInd = this->blocks->get(lastBlockInd)->getLines()->getLen() - 1,
    };

    this->top = false;
}

bool ScrollTextPane::log(const text_info_t *ti, const char *msg) {
    TextBlock *tb = 
        new TextBlock(this->getChnl(), ti, msg, this->paneInfo->lineWidth);

    size_t numLines = tb->getLines()->getLen();

    if (numLines == 0) {
        delete tb;

        return false;
    }

    if (this->blocks->getLen() == 0) {
        this->totalHeight = (numLines * ti->heightScale * 8) + 
            ((numLines - 1) * this->paneInfo->vertLineSpace);
    } else {
        this->totalHeight += numLines * 
            ((ti->heightScale * 8) + this->paneInfo->vertLineSpace);
    }

    this->blocks->add(tb);
    this->gotoBottom();

    return true;
}




