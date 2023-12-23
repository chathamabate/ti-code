
#pragma once

#include <stdint.h>
#include <cxxutil/core/mem.h>

// A view into a planet's farm???
// That's what this is???
// How should this be managed???
namespace tif { namespace ui {

    static constexpr uint8_t TILE_WIDTH = 32;
    static constexpr uint8_t TILE_HEIGHT = 32;
    static constexpr uint8_t HALF_TILE_HEIGHT = TILE_HEIGHT / 2;

    // TileRenderer should be stateless!
    class TileRenderer : public cxxutil::core::SafeObject {
    public:
        virtual ~TileRenderer();

        // Not all tiles have an excess!
        virtual bool hasExcess() const = 0;

        // For all of these calls (x, y) is the top left of the tile.
        // Not the top left of what is to be drawn.
        virtual void renderExcess(cxxutil::core::U24 x, uint8_t y) const = 0; 
        virtual void renderTop(cxxutil::core::U24 x, uint8_t y) const = 0; 
        virtual void renderBottom(cxxutil::core::U24 x, uint8_t y) const = 0; 
    };

    // Maybe the whole view is the grid...
    // That could be cool...
    // When you click, a menu pops up??
    // Except for maybe a bar at the top?
    // You click a button and it brings up options...
    // These options change the cursor color??
    // Depending on what the cursor does...
    // Ok, so there's a grid? 
    // How do we know how to render each tile tho??
    // We could make all the earth tiles now...
    // Then go from there...

    // How should this all interact tho??
    // I want the grid view to independent from the other components of the UI.
    // Do we want a menu ever to pop up??
    // No bottom menu whatsoever??
    // Everything in this grid format??
    // The ui could hold an instance of the model??
    // Render however we want???
    // Why over engineer??
    //
    // A style kinda like Minesweeper?
    //
    // Menus over the grid?
    // Planting/clearing/placing/harvesting?
    // Lots of things to consider...
    // Menus over the grid?

    // Concrete class.
    // How should the TileGrid interact with the planet which is being
    // displayed? Will there need to be some sort of manager?
    // What about selection? How will that affect this model??
    // Maybe one cell can hold multiple renderers?
    //
    class TileGrid : public cxxutil::core::SafeObject {
    private:
        // x and y of where to render the grid.
        const cxxutil::core::U24 x;
        const uint8_t y;

        // rows and columns in the grid.
        const uint8_t rows;
        const uint8_t cols;

        // Should we instead hold an offset into the state?
        // There also needs to be an actual state tbh... 

        // Grid currently rendered on the screen.
        cxxutil::core::SafeArray<const TileRenderer *> *visibleGrid;

        // Grid in the buffer.
        cxxutil::core::SafeArray<const TileRenderer *> *bufferGrid;
    public:
        TileGrid(uint8_t chnl, uint8_t rs, uint8_t cs, const TileRenderer *defTR);

         
    };

}}
