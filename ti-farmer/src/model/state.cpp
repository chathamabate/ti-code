

#include "./state.h"
#include <cxxutil/core/mem.h>
#include <cxxutil/data/bits.h>

using namespace tif::model;

SeasonState::SeasonState(const statics::season_t *s, const statics::goal_timeline_t *gt) 
    : SeasonState(cxxutil::core::CXX_DEF_CHNL, s, gt) {
}

SeasonState::SeasonState(uint8_t chnl, const statics::season_t *s, const statics::goal_timeline_t *gt) 
    : cxxutil::core::SafeObject(chnl), season(s), goalTimeline(gt) {
    
    this->cropCounts = new cxxutil::core::SafeArray<cxxutil::core::U24>(
            chnl, this->season->cropsLen);

    for (uint8_t i = 0; i < this->season->cropsLen; i++) {
        this->cropCounts->set(i, 0);    // 0 out.
    }

    this->cropLocks = new cxxutil::data::BitVector(
            chnl, this->season->cropsLen);

    // All crops start as locked.
    this->cropLocks->complement();

    // All goals start as uncollected.
    this->goals = new cxxutil::data::BitGrid(
            chnl, this->season->cropsLen, this->goalTimeline->timelineLen);
}

SeasonState::~SeasonState() {
    delete this->goals;
    delete this->cropLocks;
    delete this->cropCounts;
}

bool PlanetState::plant(uint8_t r, uint8_t c, uint8_t cropInd) {
    cell_state_t *cell = this->getCellStatePtr(r, c);

    if (!(this->isPlantablePtr(cell))) {
        return false;
    }

    // Wait... but is it unlocked yet???
    return true;
}

bool PlanetState::harvest(uint8_t r, uint8_t c) {
    return false;
}

bool PlanetState::clear(uint8_t r, uint8_t c) {
    return false;
}

bool PlanetState::place(uint8_t r, uint8_t c, uint8_t featureInd) {
    return false;
}



