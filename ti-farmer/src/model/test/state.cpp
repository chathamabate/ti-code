

// These absolute paths are a bit better now tbh.
#include "fileioc.h"
#include <ti-farmer/src/model/state.h>
#include <ti-farmer/src/model/test/state.h>
#include <ti-farmer/src/statics/earth.h>
#include <ti-farmer/src/statics/universe.h>

#include <cxxutil/unit/unit.h>
#include <stdlib.h>
#include <stdio.h>

using namespace tif::model;

// For easy use.
static const tif::statics::planet_t * const 
EARTH_PTR = &tif::statics::EARTH;

static const tif::statics::goal_timeline_t * const
GOALS_PTR = tif::statics::UNIVERSE.goalTimeline;

class SeasonStateFileCase : public cxxutil::unit::TestCase {
private:
    static SeasonStateFileCase ONLY_VAL;
    SeasonStateFileCase() : TestCase("SeasonState File Case") {}

    SeasonState *ss;
    SeasonState *resSs;

    SeasonStateFileWriter *ssWriter;
    SeasonStateFileReader *ssReader;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        const tif::statics::season_t * const S_PTR = EARTH_PTR->seasons[0];
        this->ss = new SeasonState(2, S_PTR, GOALS_PTR);
        this->resSs = NULL;
        this->ssWriter = new SeasonStateFileWriter(2);
        this->ssReader = new SeasonStateFileReader(2, S_PTR, GOALS_PTR);

        tc->assertTrue(S_PTR->cropsLen >= 3);

        this->ss->unlock(0);
        this->ss->unlock(1);

        this->ss->incCropCount(0, 10);
        this->ss->incCropCount(1, 100);

        this->ss->collect(1, 0); 

        uint8_t handle;

        handle = ti_Open("TVAR", "w");
        bool writeRes = this->ssWriter->write(handle, this->ss);
        ti_Close(handle);

        tc->assertTrue(writeRes);

        handle = ti_Open("TVAR", "r");
        bool readRes = this->ssReader->read(handle, &(this->resSs));
        ti_Close(handle);

        tc->assertTrue(readRes);

        // Now some simple equality checks.
        tc->assertTrue(this->resSs->isUnlocked(0));
        tc->assertTrue(this->resSs->isUnlocked(1));
        tc->assertFalse(this->resSs->isUnlocked(2));

        tc->assertEqUInt(10, this->resSs->getCropCount(0));
        tc->assertEqUInt(100, this->resSs->getCropCount(1));
        tc->assertEqUInt(0, this->resSs->getCropCount(2));

        tc->assertTrue(this->resSs->isCollected(1, 0));
    }

    virtual void finally() override {
        delete this->ssReader;
        delete this->ssWriter;

        if (this->resSs) {
            delete this->resSs;
        }

        delete this->ss;

        ti_Delete("TVAR");
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

SeasonStateFileCase SeasonStateFileCase::ONLY_VAL;

class PlanetStateFileCase : public cxxutil::unit::TestCase {
private:
    static PlanetStateFileCase ONLY_VAL;
    PlanetStateFileCase() : TestCase("PlanetState File Case") {}

    PlanetState *initPs;
    PlanetState *resPs;

    PlanetStateFileWriter *psWriter;
    PlanetStateFileReader *psReader;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        this->initPs = new PlanetState(2, EARTH_PTR, GOALS_PTR);
        this->resPs = NULL;

        this->psWriter = new PlanetStateFileWriter(2);
        this->psReader = new PlanetStateFileReader(2, EARTH_PTR, GOALS_PTR);
    
        // Do some work on our planet state.

        // Let's plant a crop!
        tc->assertTrue(EARTH_PTR->gridRows > 0 && 
                EARTH_PTR->gridCols > 0);

        tc->assertTrue(this->initPs->place(0, 0, 0));

        uint8_t si = this->initPs->getSeasonInd();

        tc->assertTrue(EARTH_PTR->seasons[si]->cropsLen > 0);
        tc->assertTrue(this->initPs->unlock(this->initPs->getSeasonInd(), 0));
        tc->assertTrue(this->initPs->plant(0, 0, 0));

        this->initPs->incDate();

        this->initPs->insertHighscore({.date = 10, .time = 3});
        
        bool res;
        uint8_t handle;

        handle = ti_Open("TVAR", "w");
        res = this->psWriter->write(handle, this->initPs);
        ti_Close(handle);

        tc->assertTrue(res);

        handle = ti_Open("TVAR", "r");
        res = this->psReader->read(handle, &(this->resPs));
        ti_Close(handle);

        tc->assertTrue(res);

        // Some simple equality checks.
        tc->assertTrue(this->resPs->isClearable(0, 0));
        tc->assertTrue(this->resPs->getCellState(0, 0).cropInd == 0);
        tc->assertTrue(this->resPs->getDate() == this->initPs->getDate());

        tc->assertEqUInt(10, this->resPs->getHighscore(0).date);
        tc->assertEqUInt(1, this->resPs->getHSLen());
    }

    virtual void finally() override {
        delete this->initPs;

        if (this->resPs) {
            delete this->resPs;
        }

        delete this->psWriter;
        delete this->psReader;

        ti_Delete("TVAR");
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

PlanetStateFileCase PlanetStateFileCase::ONLY_VAL;

class StateCase1 : public cxxutil::unit::TestCase {
private:
    static StateCase1 ONLY_VAL;
    StateCase1() : TestCase("State Case 1") {}

    PlanetState *ps;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        // This checks switching seasons and harvesting a crop.
        
        this->ps = new PlanetState(2, EARTH_PTR, GOALS_PTR);
        
        const tif::statics::season_t *season = this->ps->getSeason();
        
        // Let's try incrementing to the next season.
        tif::statics::day_count_t d = season->duration;

        for (tif::statics::day_count_t i = 0; i < d - 1; i++) {
            tc->assertFalse(this->ps->incDate());
        }

        tc->assertTrue(this->ps->incDate());
        tc->assertEqUInt(1, this->ps->getSeasonInd());

        season = this->ps->getSeason();
        tc->assertTrue(season->cropsLen > 0);  

        // We are going to try growing crop 0.

        // Pick a valid point on the grid.
        uint8_t r = 3;
        uint8_t c = 2;

        tc->assertTrue(r < EARTH_PTR->gridRows);
        tc->assertTrue(c < EARTH_PTR->gridCols);

        tc->assertFalse(this->ps->isClearable(r, c));
        tc->assertTrue(this->ps->isPlaceable(r, c, 0));

        // Make [r, c] a plot.
        tc->assertTrue(this->ps->place(r, c, 0));

        tc->assertFalse(this->ps->place(r, c, 0));  // Can't do twice in a row.
        tc->assertTrue(this->ps->isClearable(r, c));

        // We need to unlock first!
        tc->assertFalse(this->ps->isPlantable(r, c, 0));

        // Unlock and Plant!
        tc->assertTrue(this->ps->unlock(this->ps->getSeasonInd(), 0));
        tc->assertTrue(this->ps->plant(r, c, 0));

        tif::statics::day_count_t growTime = season->crops[0]->timeToHarvest;

        for (tif::statics::day_count_t i = 0; i < growTime; i++) {
            tc->assertFalse(ps->isHarvestable(r, c));

            this->ps->incDate();
        }

        tc->assertTrue(ps->harvest(r, c));

        tc->assertTrue(ps->getCropCount(this->ps->getSeasonInd(), 0) == 1);

        // Woo!
    }

    virtual void finally() override {
        delete ps;
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

StateCase1 StateCase1::ONLY_VAL;

class StateCase2 : public cxxutil::unit::TestCase {
private:
    static StateCase2 ONLY_VAL;
    StateCase2() : TestCase("State Case 2") {}

    PlanetState *ps;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        this->ps = new PlanetState(2, EARTH_PTR, GOALS_PTR);

        // Let's try planting a crop and advancing to the next season.
        const tif::statics::season_t *s = this->ps->getSeason();

        tc->assertTrue(s->cropsLen > 0);
        tc->assertTrue(this->ps->unlock(0, 0));     // Unlock first crop.
                                                    
        tc->assertTrue(EARTH_PTR->gridRows > 0 && EARTH_PTR->gridCols > 0);
        tc->assertTrue(this->ps->place(0, 0, 0));   // Place a plot.

        tc->assertTrue(this->ps->plant(0, 0, 0));   // Plant our crop.

        // Now, let's go to next season.
        for (tif::statics::day_count_t i = 0; i < s->duration; i++) {
            this->ps->incDate();
        }

        tc->assertEqUInt(1, this->ps->getSeasonInd());

        cell_state_t cell = this->ps->getCellState(0, 0);
        s = this->ps->getSeason();

        tc->assertEqUInt(s->cropsLen, cell.cropInd);
    }

    virtual void finally() override {
        delete ps;
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

StateCase2 StateCase2::ONLY_VAL;

class StateCase3 : public cxxutil::unit::TestCase {
private:
    static StateCase3 ONLY_VAL;
    StateCase3() : TestCase("State Case 3") {}

    PlanetState *ps;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
        this->ps = new PlanetState(2, EARTH_PTR, GOALS_PTR);

        // Now let's try purchasing/placing a bunch of features.
        tc->assertTrue(EARTH_PTR->featuresLen > 1);
        tc->assertTrue(EARTH_PTR->features[1]->initialAmt > 0);

        // We will be using feature 1.

        feature_count_t fc = this->ps->getFeatureCount(1);

        tc->assertEqUInt(EARTH_PTR->features[1]->initialAmt, fc.owned);
        tc->assertEqUInt(0, fc.placed);

        uint8_t i;

        for (i = 0; i < fc.owned; i++) {
            tc->assertTrue(this->ps->place(
                        i / EARTH_PTR->gridCols, i % EARTH_PTR->gridCols, 1));
        }

        tc->assertFalse(this->ps->place(
                    i / EARTH_PTR->gridCols, i % EARTH_PTR->gridCols, 1));

        // Now let's try to purchase to the max!

        for (i = fc.owned; i < EARTH_PTR->features[1]->maxAmt; i++) {
            tc->assertTrue(this->ps->purchase(1)); 
        }

        tc->assertFalse(this->ps->purchase(1)); 
        tc->assertFalse(this->ps->place(0, 0, 1)); // Should already be filled!
        
        // Let's try inserting some highscores.
        for (uint8_t i = 0; i <= PlanetState::HS_CAP; i++) {
            tc->assertEqUInt(i, 
                    this->ps->insertHighscore({.date = 0, .time = i}));
        }

        tc->assertEqUInt(2, 
                this->ps->insertHighscore({.date = 0, .time = 1}));

    }

    virtual void finally() override {
        delete ps;
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

StateCase3 StateCase3::ONLY_VAL;

static const size_t STATE_SUITE_TESTS_LEN = 5;
static cxxutil::unit::TestTree * const 
STATE_SUITE_TESTS[STATE_SUITE_TESTS_LEN] = {
    SeasonStateFileCase::ONLY,
    PlanetStateFileCase::ONLY,
    StateCase1::ONLY,
    StateCase2::ONLY,
    StateCase3::ONLY
};

static cxxutil::unit::TestSuite
STATE_SUITE_VALUE("State Suite", STATE_SUITE_TESTS, STATE_SUITE_TESTS_LEN);

cxxutil::unit::TestTree * const 
tif::model::STATE_SUITE = &STATE_SUITE_VALUE;
