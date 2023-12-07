
#pragma once

#include <cxxutil/core/mem.h>

#include "../statics/universe.h"
#include "cxxutil/data/bits.h"
#include "ti-farmer/src/model/universe.h"

namespace tif { namespace model {



    // A planets environent is made up of a grid of cells.
    struct cell_state_t {
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

    struct feature_count_t {
        uint8_t placed;
        uint8_t owned;
    };

    // Seasonal specific states.
    class SeasonState : public cxxutil::core::SafeObject {
    private:
        const statics::season_t * const season;
        const statics::goal_timeline_t * const goalTimeline;

        cxxutil::core::SafeArray<cxxutil::core::U24> *cropCounts;
        cxxutil::data::BitVector *cropLocks;    // 1 when locked, 0 when not.
        cxxutil::core::SafeArray<cxxutil::data::BitVector *> goals; // 1 when collected, 0 when not.

    public:
        SeasonState(const statics::season_t *s);
        SeasonState(uint8_t chnl, const statics::season_t *s);
        ~SeasonState();

        // This doesn't check if a crop is locked or not.
        inline void incCropCount(uint8_t cropInd, cxxutil::core::U24 amt) {
            *(this->cropCounts->getPtrMut(cropInd)) += amt;
        }

        inline cxxutil::core::U24 getCropCount(uint8_t cropInd) {
            return this->cropCounts->get(cropInd);
        }

        inline bool isLocked(uint8_t cropInd) {
            return this->cropLocks->get(cropInd);
        }

        bool unlock(uint8_t cropInd) {
            if (this->isLocked(cropInd)) {
                this->cropLocks->set(cropInd, 0);
                return true;
            }

            return false;
        }
        
        bool isCollected(uint8_t cropInd, uint8_t goalInd) {

        }

        bool isPassed(uint8_t cropInd, uint8_t goalInd);
        bool collect(uint8_t cropInd, uint8_t goalInd);
    };

    // Maybe the only universal thing should be stars???

    class PlanetState : public cxxutil::core::SafeObject {
    private:
        const statics::planet_t * const planet;

        // Todays date.
        statics::day_count_t date;

        // Store the season just for quick speed up.
        // Can always be calculated on demand just using date.
        const statics::season_t *season;


        // Feature inventory on this planet.
        cxxutil::core::SafeArray<feature_count_t> *featureCounts;

        // NOTE: It is garaunteed that all crops in the grid are in-season.
        // When the season changes, all crops are cleared.
        cxxutil::core::SafeArray<cell_state_t> *grid; 
        
        // NOTE: this call is somewhat dangerous.
        // It is marked const so it can be used in other const functions.
        // However, the pointer returned can be used to modify the underlying grid.
        inline cell_state_t *getCellStatePtr(uint8_t r, uint8_t c) const {
            return this->grid->getPtrMut((r * this->planet->gridCols) + c);
        }

        inline bool isPlantablePtr(const cell_state_t *cell, uint8_t cropInd) const {
            return cell->featureInd == 0 &&                 // Is the cell a plot?
                cell->cropInd == this->season->cropsLen &&  // Is the plot clear?

        }

        inline bool isHarvestablePtr(const cell_state_t *cell) const {
            return cell->featureInd == 0 &&                  // Is it a plot?
                cell->cropInd < this->season->cropsLen &&    // Is it planted?
                this->date - cell->plantDate >= this->season->crops[cell->cropInd]->timeToHarvest;  // Is it ready to harvest?
        }

        inline bool isClearablePtr(const cell_state_t *cell) const {
            return cell->featureInd < this->planet->featuresLen;
        }

        bool isPlaceablePtr(const cell_state_t *cell, const feature_count_t *fc) const {
            return cell->featureInd == this->planet->featuresLen && // Is our cell clear?
                fc->placed < fc->owned; // Do we have another feature to place?
        }
        
    public:
        PlanetState(const statics::planet_t *p); 
        PlanetState(uint8_t chnl, const statics::planet_t *p); 

        ~PlanetState();

        inline const statics::planet_t *getPlanet() const {
            return this->planet;
        }
        
        inline cell_state_t getCellState(uint8_t r, uint8_t c) const {
            return this->grid->get((this->planet->gridCols * r) + c);
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
        inline bool isPlantable(uint8_t r, uint8_t c, uint8_t cropInd) const {
            return this->isPlantablePtr(this->getCellStatePtr(r, c), cropInd);
        }

        // You can only ever plant crops in-season on a plot.
        bool plant(uint8_t r, uint8_t c, uint8_t cropInd);

        // If the given cell is a plot with a ripe crop.
        inline bool isHarvestable(uint8_t r, uint8_t c) const {
            return this->isHarvestablePtr(this->getCellStatePtr(r, c));
        }

        // Harvest crops from the given cell.
        bool harvest(uint8_t r, uint8_t c);

        // Whether or not the given cell holds a feature.
        inline bool isClearable(uint8_t r, uint8_t c) const {
            return this->isClearablePtr(this->getCellStatePtr(r, c));
        }

        // Clear a cells feature.
        bool clear(uint8_t r, uint8_t c);

        // True when we have excess of a feature in our inventory,
        // and the given cell is clear.
        bool isPlaceable(uint8_t r, uint8_t c, uint8_t featureInd) const {
            return this->isPlaceablePtr(
                    this->getCellStatePtr(r, c), this->featureCounts->getPtr(featureInd));
        }

        // Place a feature.
        bool place(uint8_t r, uint8_t c, uint8_t featureInd);

        // These calls are slightly different than the above.
        
        // Whether or not a specific goal has been collected.
        bool isCollected(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const;

        // Whether or not the requirements of a goal have been passed.
        bool isPassed(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const;

        // Collect a goal.
        bool collect(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd);
    };
}}
