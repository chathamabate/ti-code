
#include "./maze.h"

#include "cxxutil/data/bits.h"
#include "ti/screen.h"
#include <cstdlib>
#include <cxxutil/data/heap.h>

using namespace tif::model;


static cxxutil::core::U24 randomKey(const cxxutil::data::grid_coord_t &v) {
    (void)v;

    return rand();
}

// This pushes all edges that border c which are "open".
// Meaning that they are barriers that have the chance of being
// made paths.
// NOTE: rows/cols must both  be larger than 0
static void pushOpenEdges(cxxutil::data::Heap<cxxutil::data::grid_coord_t> *open, 
        cxxutil::data::grid_coord_t c, cxxutil::data::BitGrid *bg) {
    // Add West.
    if (c.c > 0 && !(bg->get(c.r, c.c - 2))) {
        open->push((cxxutil::data::grid_coord_t){
            .r = c.r,
            .c = c.c - 1
        });
    }  

    // Add East.
    if (c.c < bg->getCols() - 1 && !(bg->get(c.r, c.c + 2))) {
        open->push((cxxutil::data::grid_coord_t){
            .r = c.r,
            .c = c.c + 1
        });
    }

    // Add North.
    if (c.r > 0 && !(bg->get(c.r - 2, c.c))) {
        open->push((cxxutil::data::grid_coord_t){
            .r = c.r - 1,
            .c = c.c 
        });
    }

    // Add South.
    if (c.r < bg->getRows() - 1 && !(bg->get(c.r + 2, c.c))) {
        open->push((cxxutil::data::grid_coord_t){
            .r = c.r + 1,
            .c = c.c
        });
    }
}

cxxutil::data::BitGrid *
tif::model::createMaze(uint8_t chnl, size_t rows, size_t cols) {
    const size_t bgRows = (2 * rows) - 1;
    const size_t bgCols = (2 * cols) - 1;

    cxxutil::data::BitGrid *bg 
        = new cxxutil::data::BitGrid(chnl, bgRows, bgCols);

    // This contains coordinates of edge pathes which may
    // be addable to our maze.
    cxxutil::data::Heap<cxxutil::data::grid_coord_t> open(chnl, randomKey, 20);

    // Remember, due to how the maze is organized, an edge
    // path can only ever connect to node pathes. 

    // First lets get our random starting point.
    cxxutil::data::grid_coord_t start = {
        .r = (rand() % rows) * 2,
        .c = (rand() % cols) * 2
    };

    bg->set(start.r, start.c, true);
    pushOpenEdges(&open, start, bg);

    while (open.getLength() > 0) {
        cxxutil::data::grid_coord_t edge = open.pop();
        
        // It's possible our edge has already been visited.
        if (bg->get(edge.r, edge.c)) {
            continue;
        }

        // Given our edge was added to the open set,
        // it is gauranteed that it borders at least 1
        // path node. 
        // If it borders 2 path nodes, we do nothing.
        // Otherwise, its bordered barrier node becomes
        // a path, that nodes barrier edges are added to
        // the open set.
        
        // Invalid initial value will be used at the 
        // end to determine if a valid node was found.
        cxxutil::data::grid_coord_t node = {
            .r = bgRows,
            .c = bgCols
        };

        if (edge.r % 2 == 1) {
            // North South edges.
            
            if (!(bg->get(edge.r - 1, edge.c))) {
                // North Node.
                node = {
                    edge.r - 1,
                    edge.c
                };
            } else if (!(bg->get(edge.r + 1, edge.c))) {
                // South Node.
                node = {
                    edge.r + 1,
                    edge.c
                };
            }
        } else {
            // East West edge.

            if (!(bg->get(edge.r, edge.c - 1))) {
                // West Node.
                node = {
                    .r = edge.r,
                    .c = edge.c - 1
                };
            } else if (!(bg->get(edge.r, edge.c + 1))) {
                // East Node.
                node = {
                    .r = edge.r,
                    .c = edge.c + 1
                };
            }
        }

        // Did we find a barrier bordering node??
        // If so, make both the edge and node a path.
        // Add our nodes other barrier edges to the open set.
        if (node.r < bgRows && node.c < bgCols) {
            bg->set(edge.r, edge.c, true);
            bg->set(node.r, node.c, true);

            pushOpenEdges(&open, node, bg);
        }
    }

    return bg;
}

