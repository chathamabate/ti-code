
#include "./maze.h"

#include "cxxutil/data/bits.h"
#include "ti/screen.h"
#include <cstdlib>
#include <cxxutil/data/heap.h>
#include <cxxutil/core/data.h>

using namespace tif::model;

// DFS Code...

typedef struct {
    cxxutil::core::CoreList<cxxutil::data::grid_coord_t> *s;     // Stack.
    const cxxutil::data::BitGrid *m;                             // Maze.
    cxxutil::data::BitGrid *v;                                   // Visted.
} dfs_context_t;

// A node is marked visited once it is gauranteed all of its outgoing edges
// have been visited or will be visited in the future.
static void visitNode(dfs_context_t ctx, cxxutil::data::grid_coord_t node) {
    const size_t rows = ctx.m->getRows();
    const size_t cols = ctx.m->getCols();

    // North.
    if (node.r > 0 && ctx.m->get(node.r - 1, node.c)) {
        ctx.s->add(
                (cxxutil::data::grid_coord_t)
                {.r = node.r - 1, .c = node.c});
    }

    // South.
    if (node.r < rows - 1 && ctx.m->get(node.r + 1, node.c)) {
        ctx.s->add(
                (cxxutil::data::grid_coord_t)
                {.r = node.r + 1, .c = node.c});
    }

    // West.
    if (node.c > 0 && ctx.m->get(node.r, node.c - 1)) {
        ctx.s->add(
                (cxxutil::data::grid_coord_t)
                {.r = node.r, .c = node.c - 1});
    }

    // East.
    if (node.c < cols - 1 && ctx.m->get(node.r, node.c + 1)) {
        ctx.s->add(
                (cxxutil::data::grid_coord_t)
                {.r = node.r, .c = node.c + 1});
    }

    
    // Mark Visited!
    ctx.v->set(node.r, node.c, true);
}

// An edge is marked visited after both of its endpoints are marked visited.
static void visitEdge(dfs_context_t ctx, cxxutil::data::grid_coord_t edge) {

    // It is gauranteed one or both of the edges endpoints are visited.

    if (edge.r % 2 == 0) {
        // East West Edge.
        
        if (!(ctx.v->get(edge.r, edge.c - 1))) {

            // West Node not visited.
            visitNode(ctx, 
                    (cxxutil::data::grid_coord_t)
                    {.r = edge.r, .c = edge.c - 1});

        } else if (!(ctx.v->get(edge.r, edge.c + 1))) {

            // East Node not visited.
            visitNode(ctx, 
                    (cxxutil::data::grid_coord_t)
                    {.r = edge.r, .c = edge.c + 1});
        }
        
    } else {
        // North South Edge.

        if (!(ctx.v->get(edge.r - 1, edge.c))) {

            // North Node not visited.
            visitNode(ctx, 
                    (cxxutil::data::grid_coord_t)
                    {.r = edge.r - 1, .c = edge.c});

        } else if (!(ctx.v->get(edge.r + 1, edge.c))) {

            // South Node not visited.
            visitNode(ctx, 
                    (cxxutil::data::grid_coord_t)
                    {.r = edge.r + 1, .c = edge.c});
        }
    }

    // Mark Visited.
    ctx.v->set(edge.r, edge.c, true);
}

cxxutil::data::BitGrid *tif::model::createMazeDFS(uint8_t chnl, const cxxutil::data::BitGrid *m) {
    cxxutil::data::BitGrid *visited = 
        new cxxutil::data::BitGrid(chnl, m->getRows(), m->getCols());

    // The stack actually only holds edges.
    cxxutil::core::CoreList<cxxutil::data::grid_coord_t> stack;

    dfs_context_t ctx = {
        .s = &stack,
        .m = m,
        .v = visited,
    };

    visitNode(ctx, (cxxutil::data::grid_coord_t){.r = 0, .c = 0});

    while (stack.getLen() > 0) {
        cxxutil::data::grid_coord_t edge = stack.pop();
        
        // As we aren't necessarily working with a tree,
        // It will be possible that the same edge is added twice.
        if (visited->get(edge.r, edge.c)) {
            continue;
        } 

        visitEdge(ctx, edge);
    }

    return visited;
}

// Maze Generation Code.

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
        
        // NOTE: An edge is only added to the open set
        // when one of its end points is a path.
        //
        // This can never happen twice!
        //
        // Thus, it is gauranteed every edge in the openset
        // is a barrier.
        //
        // if (bg->get(edge.r, edge.c)) {
        //     continue;
        // }

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

            // NOTE: it is essential that this call comes
            // after the above two lines.
            // Otherwise an edge could be added to the open set twice!
            pushOpenEdges(&open, node, bg);
        }
    }

    return bg;
}

