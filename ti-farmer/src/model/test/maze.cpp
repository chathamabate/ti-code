
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
    cxxutil::data::BitGrid *visited; 

    virtual void attempt(cxxutil::unit::TestContext *tc) override {

    }

    virtual void finally() override {
        delete this->visited;
        delete this->m;
    }

public:
    MazeCase(const char *name, unsigned int s, size_t rs, size_t cs) 
        : TestCase(name), seed(s), rows(rs), cols(cs) {}
};
