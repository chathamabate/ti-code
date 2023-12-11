
#pragma once

#include <cxxutil/core/mem.h>

#include "../statics/universe.h"
#include "cxxutil/data/bits.h"
#include <cxxutil/data/file.h>

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
        friend class SeasonStateFileWriter;
        friend class SeasonStateFileReader;

    private:
        const statics::season_t * const season;
        const statics::goal_timeline_t * const goalTimeline;

        cxxutil::core::SafeArray<cxxutil::core::U24> *cropCounts;
        cxxutil::data::BitVector *cropUnlocks;  // 1 when unlocked 0 when locked.
        cxxutil::data::BitGrid *goals;          // 1 when collected, 0 when not.

        SeasonState(uint8_t chnl, const statics::season_t *s,
                const statics::goal_timeline_t *gt,
                cxxutil::core::SafeArray<cxxutil::core::U24> *cc,
                cxxutil::data::BitVector *cu,
                cxxutil::data::BitGrid *gs);

    public:
        SeasonState(const statics::season_t *s, const statics::goal_timeline_t *gt);
        SeasonState(uint8_t chnl, const statics::season_t *s, const statics::goal_timeline_t *gt);
        ~SeasonState();

        // This doesn't check if a crop is locked or not.
        inline void incCropCount(uint8_t cropInd, cxxutil::core::U24 amt) {
            *(this->cropCounts->getPtrMut(cropInd)) += amt;
        }

        inline cxxutil::core::U24 getCropCount(uint8_t cropInd) {
            return this->cropCounts->get(cropInd);
        }

        inline bool isUnlocked(uint8_t cropInd) {
            return this->cropUnlocks->get(cropInd);
        }

        bool unlock(uint8_t cropInd) {
            if (!(this->isUnlocked(cropInd))) {
                this->cropUnlocks->set(cropInd, 1);
                return true;
            }

            return false;
        }
        
        inline bool isCollected(uint8_t cropInd, uint8_t goalInd) {
            return this->goals->get(cropInd, goalInd);
        }

        inline bool isPassed(uint8_t cropInd, uint8_t goalInd) {
            return this->cropCounts->get(cropInd) >= this->goalTimeline->timeline[goalInd];
        }

        inline bool collect(uint8_t cropInd, uint8_t goalInd) {
            if ( (!(this->isCollected(cropInd, goalInd))) && this->isPassed(cropInd, goalInd) ) {
                this->goals->set(cropInd, goalInd, true);

                return true;
            }

            return false;
        }
    };

    class SeasonStateFileWriter : public cxxutil::data::FileWriter<SeasonState *> {
    public:
        SeasonStateFileWriter();    
        SeasonStateFileWriter(uint8_t chnl);
        virtual ~SeasonStateFileWriter();
        virtual bool write(uint8_t handle, SeasonState *element) override;
    };

    class SeasonStateFileReader : public cxxutil::data::FileReader<SeasonState *> {
    private:
        const statics::season_t * const season;
        const statics::goal_timeline_t * const goalTimeline;

    public:
        SeasonStateFileReader(const statics::season_t *s, const statics::goal_timeline_t *gt);    
        SeasonStateFileReader(uint8_t chnl, 
                const statics::season_t *s, const statics::goal_timeline_t *gt);

        virtual ~SeasonStateFileReader();
        virtual bool read(uint8_t handle, SeasonState **dest) override;
    };

    // Date and maze completion time of a highscore.
    struct highscore_entry_t {
        statics::day_count_t date;
        uint16_t time;
    };

    // Maybe the only universal thing should be stars???

    class PlanetState : public cxxutil::core::SafeObject {
        friend class PlanetStateFileWriter;
        friend class PlanetStateFileReader;

    public:
        // Number of high score entries to store.
        static constexpr uint8_t HS_CAP = 5;
    private:
        const statics::planet_t * const planet;

        // Number of days in a year.
        statics::day_count_t yearLen;

        // Todays date.
        statics::day_count_t date;

        uint8_t hsLen;  // Number of highscores entered.
                        // Never greater than HS_CAP.
        highscore_entry_t highscores[HS_CAP];

        // Remember that goals and crops are organzied in seasons.
        SeasonState *seasonStates[statics::NUM_SEASONS];

        // Here we store current season information for speed and
        // code brevity.
        uint8_t seasonInd;
        const statics::season_t *season;
        SeasonState *seasonState; // NOTE: Weak ptr, do not delete.

        // Feature inventory on this planet.
        cxxutil::core::SafeArray<feature_count_t> *featureCounts;

        // NOTE: It is garaunteed that all crops in the grid are in-season.
        // When the season changes, all crops are cleared.
        cxxutil::core::SafeArray<cell_state_t> *grid; 

        // Direct constructor.
        PlanetState(uint8_t chnl, const statics::planet_t *p,
                statics::day_count_t d,
                uint8_t hsl,
                const highscore_entry_t *hss,
                SeasonState * const *sss,
                cxxutil::core::SafeArray<feature_count_t> *fcs,
                cxxutil::core::SafeArray<cell_state_t> *g);
        
        // NOTE: this call is somewhat dangerous.
        // It is marked const so it can be used in other const functions.
        // However, the pointer returned can be used to modify the underlying grid.
        inline cell_state_t *getCellStatePtr(uint8_t r, uint8_t c) const {
            return this->grid->getPtrMut((r * this->planet->gridCols) + c);
        }

        inline bool isPlantablePtr(const cell_state_t *cell, uint8_t cropInd) const {
            return cell->featureInd == 0 &&                 // Is the cell a plot?
                cell->cropInd == this->season->cropsLen &&  // Is the plot clear?
                this->seasonState->isUnlocked(cropInd);     // Have we unlocked said crop.
        }

        inline bool isHarvestablePtr(const cell_state_t *cell) const {
            return cell->featureInd == 0 &&                  // Is it a plot?
                cell->cropInd < this->season->cropsLen &&    // Is it planted?
                this->date - cell->plantDate >= this->season->crops[cell->cropInd]->timeToHarvest;  // Is it ready to harvest?
        }

        inline bool isClearablePtr(const cell_state_t *cell) const {
            return cell->featureInd < this->planet->featuresLen;
        }

        inline bool isPlaceablePtr(const cell_state_t *cell, const feature_count_t *fc) const {
            return cell->featureInd == this->planet->featuresLen && // Is our cell clear?
                fc->placed < fc->owned; // Do we have another feature to place?
        }

        inline bool isPurchaseablePtr(const statics::feature_t *f, const feature_count_t *fc) const {
            return fc->owned < f->maxAmt;
        }
        
    public:
        PlanetState(const statics::planet_t *p, const statics::goal_timeline_t *gt); 
        PlanetState(uint8_t chnl, const statics::planet_t *p, const statics::goal_timeline_t *gt); 

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

        inline uint8_t getHSLen() const {
            return this->hsLen;
        }

        inline const highscore_entry_t *getHighscores() const {
            return this->highscores;
        }

        inline highscore_entry_t getHighscore(uint8_t i) const {
            return this->highscores[i];
        }

        inline uint8_t getSeasonInd() const {
            return this->seasonInd;
        }

        inline const statics::season_t *getSeason() const {
            return this->season;
        }

        // Attempt to insert a new highscore into the hs array.
        // Returns the index at which hse was inserted.
        // (Will return HS_CAP if the given entry could not be inserted)
        uint8_t insertHighscore(highscore_entry_t hse);

        // NOTE: The below calls will assume all indeces are valid
        // given grid dimmensions and season.
        //
        // NOTE: Bank balance is a universal state and thus out of
        // planet state. No method below knows anything about "tokens"
        // It is the responsibility of the universal state to make sure
        // "tokens" are handled correctly.

        // Go to tomorrow. (This will account for season changes)
        // Returns true if there's been a seasonal change.
        // false otherwise.
        bool incDate();

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
        inline bool isPlaceable(uint8_t r, uint8_t c, uint8_t featureInd) const {
            return this->isPlaceablePtr(
                    this->getCellStatePtr(r, c), this->featureCounts->getPtr(featureInd));
        }

        // Place a feature.
        bool place(uint8_t r, uint8_t c, uint8_t featureInd);

        // NOTE: Planet state knows nothing of stars/tokens/currency...
        // A feature is purchaseable iff the max amount is yet to be 
        // reached.

        inline feature_count_t getFeatureCount(uint8_t fi) const {
            return this->featureCounts->get(fi);
        }

        inline bool isPurchaseable(uint8_t fi) const {
            return this->isPurchaseablePtr(this->planet->features[fi], 
                    this->featureCounts->getPtr(fi));
        }

        bool purchase(uint8_t fi);

        // Wrappers around Season State calls.
        inline cxxutil::core::U24 getCropCount(uint8_t seasonInd, uint8_t cropInd) const {
            return this->seasonStates[seasonInd]->getCropCount(cropInd);
        }

        inline bool isUnlocked(uint8_t seasonInd, uint8_t cropInd) const {
            return this->seasonStates[seasonInd]->isUnlocked(cropInd);
        }

        inline bool unlock(uint8_t seasonInd, uint8_t cropInd) {
            return this->seasonStates[seasonInd]->unlock(cropInd);
        }

        // These calls are slightly different than the above.
        
        // Whether or not a specific goal has been collected.
        inline bool isCollected(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const {
            return this->seasonStates[seasonInd]->isCollected(cropInd, goalInd);
        }

        // Whether or not the requirements of a goal have been passed.
        inline bool isPassed(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) const {
            return this->seasonStates[seasonInd]->isPassed(cropInd, goalInd);
        }

        // Collect a goal.
        inline bool collect(uint8_t seasonInd, uint8_t cropInd, uint8_t goalInd) {
            return this->seasonStates[seasonInd]->collect(cropInd, goalInd);
        }
    };

    class PlanetStateFileWriter : public cxxutil::data::FileWriter<PlanetState *> {
    public:
        PlanetStateFileWriter();    
        PlanetStateFileWriter(uint8_t chnl);
        virtual ~PlanetStateFileWriter();
        virtual bool write(uint8_t handle, PlanetState *element) override;
    };

    class PlanetStateFileReader : public cxxutil::data::FileReader<PlanetState *> {
    private:
        const statics::planet_t * const planet;
        const statics::goal_timeline_t * const goalTimeline;
    public:
        PlanetStateFileReader(const statics::planet_t *p, const statics::goal_timeline_t *gt);
        PlanetStateFileReader(uint8_t chnl, const statics::planet_t *p, const statics::goal_timeline_t *gt);

        virtual ~PlanetStateFileReader();
        virtual bool read(uint8_t handle, PlanetState **dest) override;
    };
}}
