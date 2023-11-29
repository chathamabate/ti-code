
#pragma once

#include <cxxutil/core/mem.h>
#include <cxxutil/data/bits.h>

namespace tif { namespace model {

    // I don't even think maze should be its own class tbh...
    // It's really just a bit grid in the end.
    
    // A Maze is a bit grid with dimmensions (2r - 1) x (2c - 1)
    // 0's in the grid represent barriers.
    // 1's in the grid represent paths.
    //
    // Coordinates of the form (x, y) where x, y are both even are always paths.
    // Coordinates of the form (x, y) where x, y are both odd are always barriers.
    //
    // NOTE: it is the user's responsibilty to delete the given bit grid.
    cxxutil::data::BitGrid *createMaze(uint8_t chnl, size_t rows, size_t cols);

}}
