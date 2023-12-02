
#include "./maze.h"
#include "../maze.h"
#include <cxxutil/data/bits.h>
#include <cxxutil/unit/unit.h>
#include <cxxutil/core/data.h>
#include <stdlib.h>

using namespace tif::model;

class DFSCase : public cxxutil::unit::TestCase {
private:
    const size_t rows;
    const size_t cols;
    const bool * const initMaze;
    const bool * const dfsMaze;

    cxxutil::data::BitGrid *initGrid;
    cxxutil::data::BitGrid *actualDFSGrid;
    cxxutil::data::BitGrid *dfsGrid;

    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        this->initGrid = new cxxutil::data::BitGrid(2, this->rows, this->cols);
        this->dfsGrid = new cxxutil::data::BitGrid(2, this->rows, this->cols);
        
        // Let's load up our input grids.
        const size_t inputLen  = this->rows * this->cols;
        for (size_t i = 0; i < inputLen; i++) {
            this->initGrid->set(i / cols, i % cols, this->initMaze[i]);
            this->dfsGrid->set(i / cols, i % cols, this->dfsMaze[i]);
        }

        this->actualDFSGrid = createMazeDFS(2, this->initGrid);
        
        tc->assertTrue(*(this->dfsGrid) == *(this->actualDFSGrid)); 
    }

    virtual void finally() override {
        delete this->actualDFSGrid;
        delete this->dfsGrid;
        delete this->initGrid;
    }

public:
    DFSCase(const char *name, size_t rs, size_t cs, 
            const bool *initM, const bool *dfsM) 
        : TestCase(name), rows(rs), cols(cs), initMaze(initM), dfsMaze(dfsM) {}
};

// Time to make some mazes!

static DFSCase DFS_CASE1("DFS Case 1", 1, 1, 
        (bool[]){
            true
        }, 
        (bool[]){
            true
        });

static DFSCase DFS_CASE2("DFS Case 2", 3, 3, 
        (bool[]){
            true, true, true,
            false, false, false,
            true, true, true,
        }, 
        (bool[]){
            true, true, true,
            false, false, false,
            false, false, false,
        });

static DFSCase DFS_CASE3("DFS Case 3", 3, 3, 
        (bool[]){
            true, true, true,
            true, false, false,
            true, true, true,
        }, 
        (bool[]){
            true, true, true,
            true, false, false,
            true, true, true,
        });

static DFSCase DFS_CASE4("DFS Case 4", 3, 3, 
        (bool[]){
            true, true, true,
            true, false, true,
            true, true, true,
        }, 
        (bool[]){
            true, true, true,
            true, false, true,
            true, true, true,
        });

static DFSCase DFS_CASE5("DFS Case 5", 3, 5, 
        (bool[]){
            true, true, true, false, true,
            true, false, true, false, true,
            true, true, true, false, true,
        }, 
        (bool[]){
            true, true, true, false, false,
            true, false, true, false, false,
            true, true, true, false, false
        });

static DFSCase DFS_CASE6("DFS Case 6", 3, 5, 
        (bool[]){
            true, true, true, true, true,
            true, false, true, false, true,
            true, true, true, false, true,
        }, 
        (bool[]){
            true, true, true, true, true,
            true, false, true, false, true,
            true, true, true, false, true,
        });

static DFSCase DFS_CASE7("DFS Case 7", 3, 5, 
        (bool[]){
            true, true, true, true, true,
            true, false, true, false, true,
            true, true, true, true, true,
        }, 
        (bool[]){
            true, true, true, true, true,
            true, false, true, false, true,
            true, true, true, true, true,
        });

static const size_t DFS_SUITE_TESTS_LEN = 7;
static cxxutil::unit::TestTree * const DFS_SUITE_TESTS[] = {
    &DFS_CASE1,
    &DFS_CASE2,
    &DFS_CASE3,
    &DFS_CASE4,
    &DFS_CASE5,
    &DFS_CASE6,
    &DFS_CASE7,
};

static cxxutil::unit::TestSuite DFS_SUITE_VALUE("DFS Suite", 
        DFS_SUITE_TESTS, DFS_SUITE_TESTS_LEN);

class TreeMazeCase : public cxxutil::unit::TestCase {
private:
    const size_t seed;
    const size_t rows;
    const size_t cols;

    const cxxutil::data::BitGrid *maze;
    const cxxutil::data::BitGrid *search;

    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        srand(this->seed);
        this->maze = createTreeMaze(2, this->rows, this->cols);
        this->search = createMazeDFS(2, this->maze);

        // NOTE: As we are testing our treemaze algorithm.
        // The generated maze and the searched maze should be equal.
        tc->assertTrue(*(this->maze) == *(this->search));
    }

    virtual void finally() override {
        delete this->search;
        delete this->maze;
    }

public:
    TreeMazeCase(const char *name, size_t s, size_t rs, size_t cs) 
        : TestCase(name), seed(s), rows(rs), cols(cs) {}
};

static TreeMazeCase TM_CASE1("TreeMaze Case 1", 123, 1, 1);
static TreeMazeCase TM_CASE2("TreeMaze Case 2", 123, 2, 2);
static TreeMazeCase TM_CASE3("TreeMaze Case 3", 45, 2, 2);
static TreeMazeCase TM_CASE4("TreeMaze Case 4", 45, 5, 5);
static TreeMazeCase TM_CASE5("TreeMaze Case 5", 76, 5, 5);
static TreeMazeCase TM_CASE6("TreeMaze Case 6", 76, 10, 10);

static const size_t TM_SUITE_TESTS_LEN = 6;
static cxxutil::unit::TestTree * const 
TM_SUITE_TESTS[TM_SUITE_TESTS_LEN] = {
    &TM_CASE1,
    &TM_CASE2,
    &TM_CASE3,
    &TM_CASE4,
    &TM_CASE5,
    &TM_CASE6,
};

static cxxutil::unit::TestSuite 
TM_SUITE_VALUE("TreeMaze Suite", TM_SUITE_TESTS, TM_SUITE_TESTS_LEN);

static const size_t MAZE_SUITE_TESTS_LEN = 2;
static cxxutil::unit::TestTree * const
MAZE_SUITE_TESTS[MAZE_SUITE_TESTS_LEN] = {
    &DFS_SUITE_VALUE,
    &TM_SUITE_VALUE,
};

static cxxutil::unit::TestSuite 
MAZE_SUITE_VALUE("Maze Suite", MAZE_SUITE_TESTS, MAZE_SUITE_TESTS_LEN);

cxxutil::unit::TestTree * const tif::model::MAZE_SUITE = &MAZE_SUITE_VALUE;
