
#include "./maze.h"
#include "../maze.h"
#include "cxxutil/data/bits.h"
#include <cxxutil/unit/unit.h>
#include <cxxutil/core/data.h>

using namespace tif::model;

// This test confirms that all positions in the maze are reachable
// from starting position (0, 0).
//
// NOTE: while our current maze algorithm builds a tree,
// this test case should be independent of that fact.
// The given maze could contain a loop, that is ok.
//
// A Maze is really just a connected graph of a grid of
// nodes.
class MazeCase : public cxxutil::unit::TestCase {
private:

    const unsigned int seed;
    const size_t rows;
    const size_t cols;

    // m is our maze.
    cxxutil::data::BitGrid *m;

    // Which nodes we have visited.
    cxxutil::data::BitGrid *visited; 

    cxxutil::core::CoreList<cxxutil::data::grid_coord_t> *stack;

    void pushUnvisitedNeighbors(cxxutil::data::grid_coord_t node) {
        
    }

    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        this->m = createMaze(2, this->rows, this->cols);

        this->visited = new cxxutil::data::BitGrid(2, this->rows, this->cols);
        this->stack = 
            new cxxutil::core::CoreList<cxxutil::data::grid_coord_t>(2);

        this->stack->add((cxxutil::data::grid_coord_t){.r = 0, .c = 0});

        // First we search the entire maze starting at the top left corner.
        while (this->stack->getLen() > 0) {
            cxxutil::data::grid_coord_t node = this->stack->pop(); 

            // Add all reachable unvisited neighbors to the stack!

            // Coordinates of our node in the node grid.
            size_t nodeR = node.r / 2;
            size_t nodeC = node.c / 2;

            this->visited->set(nodeR, nodeC, true);


            // Since this is a tree... 
            // There should never be the possibility of double visiting?
        }

    }

    virtual void finally() override {
        delete this->visited;
        delete this->m;
    }

public:
    MazeCase(const char *name, unsigned int s, size_t rs, size_t cs) 
        : TestCase(name), seed(s), rows(rs), cols(cs) {}
};
