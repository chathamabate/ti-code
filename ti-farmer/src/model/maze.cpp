
#include "./maze.h"
#include "ti/screen.h"

using namespace tif::model;

cxxutil::data::BitGrid *
tif::model::createMaze(uint8_t chnl, size_t rows, size_t cols) {
    const size_t bgRows = (2 * rows) - 1;
    const size_t bgCols = (2 * cols) - 1;

    cxxutil::data::BitGrid *bg 
        = new cxxutil::data::BitGrid(chnl, bgRows, bgCols);

    // Carve out gauranteed paths.
    for (size_t r = 0; r < bgRows; r += 2) {
        for (size_t c = 0; c < bgRows; c += 2) {
            bg->set(r, c, true);
        }
    }

    return bg;
}

