
#pragma once

#include <cxxutil/core/mem.h>
#include <cxxutil/data/bits.h>

namespace tif { namespace model {

    // A Maze is a bit grid with dimmensions (2r - 1) x (2c - 1)
    // 0's in the grid represent barriers.
    // 1's in the grid represent paths.
    //
    // A Maze is valid iff the paths in the maze form a single connected graph.
    // Cycles are allowed. (Although the current algorithm produces a tree).
    //
    // Coordinates of the form (x, y) where x, y are both even are always paths.
    // Coordinates of the form (x, y) where x, y are both odd are always barriers.
    //
    // NOTE: it is the user's responsibilty to delete the given bit grid.
    cxxutil::data::BitGrid *createMaze(uint8_t chnl, size_t rows, size_t cols);

    // Given a valid maze, this function traverses the maze starting at (0, 0).
    // It returns a new BitGrid where 1 represents a path which was visited.
    //
    // This function is primarily for testing purposes.
    //
    // Calling createMazeDFS on a valid maze should always return a copy
    // of the given maze.
    //
    // NOTE: it is the user's responsibilty to delete the given bit grid.
    cxxutil::data::BitGrid *createMazeDFS(uint8_t chnl, cxxutil::data::BitGrid *m);
}}
