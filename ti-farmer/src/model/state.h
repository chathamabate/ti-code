
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

    struct FeatureCount {
        uint8_t placed;
        uint8_t owned;
    };

    // Maybe the only universal thing should be stars???

    class PlanetState : public cxxutil::core::SafeObject {
    private:
        const statics::planet_t * const planet;

        // Todays date.
        statics::day_count_t date;

        // NOTE: we are going to be using a 1D array approach for
        // all dynamic arrays in this state.
        
        cxxutil::core::SafeArray<cxxutil::core::U24> *cropCounts;

        cxxutil::core::SafeArray<bool> * *goals;

        // Feature inventory on this planet.
        cxxutil::core::SafeArray<FeatureCount> *featureCounts;

        cxxutil::core::SafeArray<CellState> *grid; 
        
    public:
        PlanetState(const statics::planet_t *p); 
        PlanetState(uint8_t chnl, const statics::planet_t *p); 

        ~PlanetState();

        inline const statics::planet_t *getPlanet() const {
            return this->planet;
        }
        
        inline CellState getCellState(uint8_t r, uint8_t c) const {
            return this->grid->get(r)->get(c);
        }

        inline statics::day_count_t getDate() const {
            return this->date;
        }

        // NOTE: The below calls will assume all indeces are valid
        // given grid dimmensions and season.
        //
        // NOTE: Bank balance is a universal state and thus out of
        // planet state. No method below knows anything about "tokens"
        // It is the responsibility of the universal state to make sure
        // "tokens" are handled correctly.

        // Go to tomorrow. (This will account for season changes)
        void incDate();

        // Whether or not the given cell is an empty plot.
        bool isPlantable(uint8_t r, uint8_t c) const;

        // You can only ever plant crops in-season on a plot.
        bool plant(uint8_t r, uint8_t c, uint8_t cropInd);

        // If the given cell is a plot with a ripe crop.
        bool isHarvestable(uint8_t r, uint8_t c) const;

        // Harvest crops from the given cell.
        bool harvest(uint8_t r, uint8_t c);

        // Whether or not the given cell holds a feature.
        bool isClearable(uint8_t r, uint8_t c) const;

        // Clear a cells feature.
        bool clear(uint8_t r, uint8_t c);

        // True when we have excess of a feature in our inventory,
        // and the given cell is clear.
        bool isPlaceable(uint8_t r, uint8_t c, uint8_t featureInd) const;

        // Place a feature.
        bool place(uint8_t r, uint8_t c, uint8_t featureInd);
        
        // Whether or not a specific goal has been collected.
        bool isCollected(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const;

        // Whether or not the requirements of a goal have been passed.
        bool isPassed(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const;

        // Collect a goal.
        bool collect(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd);
    };
}}
