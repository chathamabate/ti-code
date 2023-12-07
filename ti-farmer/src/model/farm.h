
#pragma once

#include <cxxutil/core/mem.h>

#include "../statics/universe.h"

namespace tif { namespace model {

    // A planets environent is made up of a grid of cells.
    struct CellState {
        // The index of what feature occupies this cell.
        // 0 = Plot.
        // ... Custom
        // Len = empty.
        uint8_t featureInd; 

        // NOTE: these fields are only used if the Cell
        // is a plot.
        // Len = no crop planted.
        uint8_t cropInd;
        statics::day_count_t plantDate;
    };

    class PlanetState : public cxxutil::core::SafeObject {
    private:
        const statics::planet_t * const planet;
    
        cxxutil::core::SafeArray<cxxutil::core::SafeArray<CellState> *> *grid; 
        
    public:
        PlanetState(const statics::planet_t *p); 
        PlanetState(uint8_t chnl, const statics::planet_t *p); 

        ~PlanetState();

        inline const statics::planet_t *getPlanet() const {
            return this->planet;
        }
        
        inline CellState *getCellState(size_t r, size_t c) {
            return this->grid->get(r)->getPtrMut(c);
        }

    };
}}
