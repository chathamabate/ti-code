

#include "./state.h"
#include "cxxutil/data/file.h"
#include "ti-farmer/src/statics/universe.h"
#include <cxxutil/core/mem.h>
#include <cxxutil/data/bits.h>

using namespace tif::model;

SeasonState::SeasonState(uint8_t chnl, const statics::season_t *s,
                const statics::goal_timeline_t *gt,
                cxxutil::core::SafeArray<cxxutil::core::U24> *cc,
                cxxutil::data::BitVector *cu,
                cxxutil::data::BitGrid *gs) 
    : cxxutil::core::SafeObject(chnl),
      season(s), goalTimeline(gt), cropCounts(cc), cropUnlocks(cu), goals(gs) {
}

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

    this->cropUnlocks = new cxxutil::data::BitVector(
            chnl, this->season->cropsLen);

    // All goals start as uncollected.
    this->goals = new cxxutil::data::BitGrid(
            chnl, this->season->cropsLen, this->goalTimeline->timelineLen);
}

SeasonState::~SeasonState() {
    delete this->goals;
    delete this->cropUnlocks;
    delete this->cropCounts;
}

// Time for Season State File Stuff.

SeasonStateFileWriter::SeasonStateFileWriter() 
    : SeasonStateFileWriter(cxxutil::core::CXX_DEF_CHNL) {
}

SeasonStateFileWriter::SeasonStateFileWriter(uint8_t chnl) 
    : cxxutil::data::FileWriter<SeasonState *>(chnl) {
}

SeasonStateFileWriter::~SeasonStateFileWriter() {
}

bool SeasonStateFileWriter::write(uint8_t handle, SeasonState *element) {
    // All we write care crop counts, crop unlocks, and goals.

    bool res;

    cxxutil::data::ShallowArrayFileWriter<cxxutil::core::U24> 
        ccWriter(this->getChnl());

    res = ccWriter.write(handle, element->cropCounts);

    if (!res) {
        return false;
    }

    cxxutil::data::BitVectorFileWriter cuWriter(this->getChnl());
    res = cuWriter.write(handle, element->cropUnlocks);

    if (!res) {
        return false;
    }

    cxxutil::data::BitGridFileWriter gWriter(this->getChnl());
    res = gWriter.write(handle, element->goals);

    return res;
}

SeasonStateFileReader::SeasonStateFileReader(const statics::season_t *s, const statics::goal_timeline_t *gt) 
    : SeasonStateFileReader(cxxutil::core::CXX_DEF_CHNL, s, gt) {
}

SeasonStateFileReader::SeasonStateFileReader(uint8_t chnl, 
        const statics::season_t *s, const statics::goal_timeline_t *gt) 
    : cxxutil::data::FileReader<SeasonState *>(chnl), 
    season(s), goalTimeline(gt) {
}

SeasonStateFileReader::~SeasonStateFileReader() {
}

bool SeasonStateFileReader::read(uint8_t handle, SeasonState **dest) {
    cxxutil::core::SafeArray<cxxutil::core::U24> *cc = NULL;
    cxxutil::data::BitVector *cu = NULL;
    cxxutil::data::BitGrid *gs = NULL;

    bool res;

    cxxutil::data::ShallowArrayFileReader<cxxutil::core::U24>
       ccReader(this->getChnl()); 
    res = ccReader.read(handle, &cc); 

    if (!res) {
        return false;
    }

    cxxutil::data::BitVectorFileReader 
        cuReader(this->getChnl());
    res = cuReader.read(handle, &cu);

    if (!res) {
        delete cc;
        return false;
    }

    cxxutil::data::BitGridFileReader
        gsReader(this->getChnl());
    res = gsReader.read(handle, &gs);

    if (!res) {
        delete cc;
        delete cu;
        return false;
    }

    SeasonState *ss = new SeasonState(
            this->getChnl(), this->season, this->goalTimeline,
            cc, cu, gs);

    *dest = ss;

    return true;
}


// Time for planet state boys.

PlanetState::PlanetState(const statics::planet_t *p, 
        const statics::goal_timeline_t *gt) 
    : PlanetState(cxxutil::core::CXX_DEF_CHNL, p, gt) {
}

PlanetState::PlanetState(uint8_t chnl, const statics::planet_t *p, 
        const statics::goal_timeline_t *gt) 
    : cxxutil::core::SafeObject(chnl), planet(p) {

    this->yearLen = 0;
    this->date = 0; 
    this->hsLen = 0;

    // Create all seasons.
    for (uint8_t i = 0; i < statics::NUM_SEASONS; i++) {
        this->yearLen += this->planet->seasons[i]->duration;

        this->seasonStates[i] = 
            new SeasonState(chnl, this->planet->seasons[i], gt);  
    }

    // Always starts on season 0.
    this->seasonInd = 0;
    this->season = this->planet->seasons[0];
    this->seasonState = this->seasonStates[0];

    this->featureCounts = new cxxutil::core::SafeArray<feature_count_t>(
            chnl, this->planet->featuresLen);
    
    // Initialize feature counts.
    for (uint8_t i = 0; i < this->planet->featuresLen; i++) {
        feature_count_t *fc = this->featureCounts->getPtrMut(i);
        fc->placed = 0;
        fc->owned = this->planet->features[i]->initialAmt;
    }

    const size_t gridLen = this->planet->mazeRows * this->planet->mazeCols;

    // Now time to make our grid.
    this->grid = new cxxutil::core::SafeArray<cell_state_t>(
            chnl, gridLen);

    for (size_t i = 0; i < gridLen; i++) {
        cell_state_t *cell = this->grid->getPtrMut(i);
        cell->featureInd = this->planet->featuresLen;
    }

    // And we're done! (I think)
}

PlanetState::~PlanetState() {
    delete this->grid;
    delete this->featureCounts;

    for (uint8_t i = 0; i < statics::NUM_SEASONS; i++) {
        delete this->seasonStates[i];
    }
}

bool PlanetState::incDate() {
    // Advance our day.
    this->date++;
    statics::day_count_t day = this->date % this->yearLen;

    // Determine what season we are in.
    uint8_t si;
    statics::day_count_t bound = 0;
    for (si = 0; si < statics::NUM_SEASONS; si++) {
        bound += this->planet->seasons[si]->duration; 

        if (day < bound) {
            break;
        }
    }

    // No season change.
    if (si == this->seasonInd) {
        return false;
    }

    this->seasonInd = si;
    this->season = this->planet->seasons[si];
    this->seasonState = this->seasonStates[si];

    // Season change, clear plots!
    const size_t gridLen = this->grid->getLen(); 
    for (size_t i = 0; i < gridLen; i++) {
        cell_state_t *cell = this->grid->getPtrMut(i);

        // Only clear our plots.
        if (cell->featureInd == 0) {
            cell->cropInd = this->season->cropsLen;
        }
    }

    return true;
}

bool PlanetState::plant(uint8_t r, uint8_t c, uint8_t cropInd) {
    cell_state_t *cell = this->getCellStatePtr(r, c);

    if (!(this->isPlantablePtr(cell, cropInd))) {
        return false;
    }

    cell->plantDate = this->date;
    cell->cropInd = cropInd;

    return true;
}

bool PlanetState::harvest(uint8_t r, uint8_t c) {
    cell_state_t *cell = this->getCellStatePtr(r, c);

    if (!(this->isHarvestablePtr(cell))) {
        return false;
    }

    // Add crop to stock pile
    this->seasonState->incCropCount(cell->cropInd, 1);

    // Set back to empty plot.
    cell->cropInd = this->season->cropsLen;
    
    return true;
}

bool PlanetState::clear(uint8_t r, uint8_t c) {
    cell_state_t *cell = this->getCellStatePtr(r, c);

    if (!(this->isClearablePtr(cell))) {
        return false;
    }

    uint8_t fi = cell->featureInd;
    this->featureCounts->getPtrMut(fi)->placed--;

    cell->featureInd = this->planet->featuresLen;

    return true;
}

bool PlanetState::place(uint8_t r, uint8_t c, uint8_t featureInd) {
    cell_state_t *cell = this->getCellStatePtr(r, c);
    feature_count_t *fc = this->featureCounts->getPtrMut(featureInd);

    if (!(this->isPlaceablePtr(cell, fc))) {
        return false;
    }

    cell->featureInd = featureInd;

    // If we are creating a plot, make sure to clear
    // crop information fields.
    if (featureInd == 0) {
        cell->cropInd = this->season->cropsLen;
    }
    
    fc->placed++;

    return true;
}

bool PlanetState::purchase(uint8_t fi) {
    feature_count_t *fc = this->featureCounts->getPtrMut(fi);
    const statics::feature_t *f = this->planet->features[fi];

    if (!(this->isPurchaseablePtr(f, fc))) {
        return false;
    }

    fc->owned++;

    return true;
}


