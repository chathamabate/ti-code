

#include "./state.h"
#include "cxxutil/data/file.h"
#include "fileioc.h"
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

// Helper functions for calculating season Index and year length.

static tif::statics::day_count_t 
calcYearLen(const tif::statics::planet_t *p) {
    tif::statics::day_count_t yearLen = 0;
    for (uint8_t i = 0; i < tif::statics::NUM_SEASONS; i++) {
        yearLen += p->seasons[i]->duration;
    }

    return yearLen;
}

static uint8_t calcSeasonInd(const tif::statics::planet_t *p, 
        tif::statics::day_count_t day) {
    uint8_t si;
    tif::statics::day_count_t bound = 0;
    for (si = 0; si < tif::statics::NUM_SEASONS; si++) {
        bound += p->seasons[si]->duration; 

        if (day < bound) {
            return si;
        }
    }

    // This should never run.
    return si;
}

// Time for planet state boys.
PlanetState::PlanetState(uint8_t chnl, const statics::planet_t *p,
                statics::day_count_t d,
                uint8_t hsl,
                const highscore_entry_t *hss,
                SeasonState * const *sss,
                cxxutil::core::SafeArray<feature_count_t> *fcs,
                cxxutil::core::SafeArray<cell_state_t> *g) 
    : cxxutil::core::SafeObject(chnl), planet(p) {
    // Calculate year length.
    this->yearLen = calcYearLen(this->planet);

    this->date = d;
    this->hsLen = hsl;
    for (uint8_t i = 0; i < HS_CAP; i++) {
        this->highscores[i] = hss[i];
    }

    for (uint8_t i = 0; i < statics::NUM_SEASONS; i++) {
        this->seasonStates[i] = sss[i];
    }

    this->seasonInd = calcSeasonInd(this->planet, this->date % this->yearLen);
    this->season = this->planet->seasons[this->seasonInd];
    this->seasonState = this->seasonStates[this->seasonInd];

    this->featureCounts = fcs;
    this->grid = g;
}

PlanetState::PlanetState(const statics::planet_t *p, 
        const statics::goal_timeline_t *gt) 
    : PlanetState(cxxutil::core::CXX_DEF_CHNL, p, gt) {
}

PlanetState::PlanetState(uint8_t chnl, const statics::planet_t *p, 
        const statics::goal_timeline_t *gt) 
    : cxxutil::core::SafeObject(chnl), planet(p) {

    this->yearLen = calcYearLen(this->planet);
    this->date = 0; 
    this->hsLen = 0;

    // Create all seasons.
    for (uint8_t i = 0; i < statics::NUM_SEASONS; i++) {
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

uint8_t PlanetState::insertHighscore(highscore_entry_t hse) {
    // Find out at what index hse belongs.
    uint8_t i;

    for (i = 0; i < this->hsLen; i++) {
        if (this->highscores[i].time > hse.time) {
            break;
        }
    }

    // No insertion.
    if (i == HS_CAP) {
        return HS_CAP;
    }

    // Shift right.
    for (uint8_t j = HS_CAP - 1; i < j; j--) {
        this->highscores[j] = this->highscores[j + 1];
    }

    // Insert.
    this->highscores[i] = hse;

    // Are we not at capactiy yet?
    if (this->hsLen < HS_CAP) {
        this->hsLen++;
    }

    return i;
}

bool PlanetState::incDate() {
    // Advance our day.
    this->date++;
    statics::day_count_t day = this->date % this->yearLen;

    // Determine what season we are in.
    uint8_t si = calcSeasonInd(this->planet, day);

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

PlanetStateFileWriter::PlanetStateFileWriter() 
    : PlanetStateFileWriter(cxxutil::core::CXX_DEF_CHNL) {
}

PlanetStateFileWriter::PlanetStateFileWriter(uint8_t chnl) 
    : cxxutil::data::FileWriter<PlanetState *>(chnl) {
}

PlanetStateFileWriter::~PlanetStateFileWriter() {
}

bool PlanetStateFileWriter::write(uint8_t handle, PlanetState *element) {
    // Order:
    // date, hsLen, highscores, seasonStates, featureCounts, grid

    const tif::statics::day_count_t date = element->getDate();
    size_t dateRes = ti_Write(&date, sizeof(tif::statics::day_count_t), 1, handle);
    if (dateRes != 1) {
        return false;
    }

    const uint8_t hsLen = element->getHSLen();
    size_t hsLenRes = ti_Write(&hsLen, sizeof(uint8_t), 1, handle);
    if (hsLenRes != 1) {
        return false;
    }

    const highscore_entry_t *highscores = element->getHighscores();
    size_t hsRes = ti_Write(highscores, sizeof(highscore_entry_t), PlanetState::HS_CAP, handle);
    if (hsRes != PlanetState::HS_CAP) {
        return false;
    }

    SeasonStateFileWriter seasonWriter(this->getChnl());
    for (uint8_t i = 0; i < tif::statics::NUM_SEASONS; i++) {
        bool seasonRes = seasonWriter.write(handle, element->seasonStates[i]);

        if (!seasonRes) {
            return false;
        }
    }

    cxxutil::data::ShallowArrayFileWriter<feature_count_t> 
       featureWriter(this->getChnl()); 
    bool featureRes = featureWriter.write(handle, element->featureCounts);
    if (!featureRes) {
        return false;
    }

    cxxutil::data::ShallowArrayFileWriter<cell_state_t>
        gridWriter(this->getChnl());
    bool gridRes = gridWriter.write(handle, element->grid);

    return gridRes;
}

PlanetStateFileReader::PlanetStateFileReader(const statics::planet_t *p, const statics::goal_timeline_t *gt) 
    : PlanetStateFileReader(cxxutil::core::CXX_DEF_CHNL, p, gt) {
}

PlanetStateFileReader::PlanetStateFileReader(uint8_t chnl, const statics::planet_t *p, const statics::goal_timeline_t *gt) 
    : cxxutil::data::FileReader<PlanetState *>(chnl), planet(p), goalTimeline(gt) {
}

PlanetStateFileReader::~PlanetStateFileReader() {
}

// Helper function for deallocating dynamics when there
// is an error during the read.
static void psfrFailureDealloc(SeasonState * const *sss,
        cxxutil::core::SafeArray<feature_count_t> *fcs,
        cxxutil::core::SafeArray<cell_state_t> *g) {
    for (uint8_t i = 0; i < tif::statics::NUM_SEASONS; i++) {
        if (sss[i]) {
            delete sss[i];
        }
    }

    if (fcs) {
        delete fcs;
    }

    if (g) {
        delete g;
    }
}

bool PlanetStateFileReader::read(uint8_t handle, PlanetState **dest) {
    // Order:
    // date, hsLen, highscores, seasonStates, featureCounts, grid

    tif::statics::day_count_t date;
    size_t dateRes = ti_Read(&date, sizeof(tif::statics::day_count_t), 1, handle);
    if (dateRes != 1) {
        return false;
    }

    uint8_t hsl;
    size_t hslRes = ti_Read(&hsl, sizeof(uint8_t), 1, handle);
    if (hslRes != 1) {
        return false;
    }

    highscore_entry_t hss[PlanetState::HS_CAP];
    size_t hssRes = ti_Read(hss, sizeof(highscore_entry_t), PlanetState::HS_CAP, handle);
    if (hssRes != PlanetState::HS_CAP) {
        return false;
    }

    // Time for dynamics.

    // Gotta make sure all of these are delete at the end though?
    SeasonState *sss[tif::statics::NUM_SEASONS];
    for (uint8_t i = 0; i < tif::statics::NUM_SEASONS; i++) {
        sss[i] = NULL;
    }
    cxxutil::core::SafeArray<feature_count_t> *fcs = NULL;
    cxxutil::core::SafeArray<cell_state_t> *g = NULL;

    for (uint8_t i = 0; i < tif::statics::NUM_SEASONS; i++) {
        SeasonStateFileReader ssReader(
                this->getChnl(), this->planet->seasons[i], this->goalTimeline);

        bool ssRes = ssReader.read(handle, &(sss[i]));
        if (!ssRes) {
            psfrFailureDealloc(sss, fcs, g);
            return false;
        }
    }

    cxxutil::data::ShallowArrayFileReader<feature_count_t>
        featureReader(this->getChnl());
    bool featureRes = featureReader.read(handle, &fcs);
    if (!featureRes) {
        psfrFailureDealloc(sss, fcs, g);
        return false;
    }

    cxxutil::data::ShallowArrayFileReader<cell_state_t>
        gridReader(this->getChnl());
    bool gridRes = gridReader.read(handle, &g);
    if (!gridRes) {
        psfrFailureDealloc(sss, fcs, g);
        return false;
    }

    // Success!

    PlanetState *ps = new PlanetState(this->getChnl(), this->planet,
            date, 
            hsl,
            hss,
            sss,
            fcs,
            g);

    *dest = ps;

    return true;
}



