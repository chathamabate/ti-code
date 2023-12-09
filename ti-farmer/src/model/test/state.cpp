

// These absolute paths are a bit better now tbh.
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

class StateCase1 : public cxxutil::unit::TestCase {
private:
    static StateCase1 ONLY_VAL;
    StateCase1() : TestCase("State Case 1") {}

    PlanetState *ps;
    
    virtual void attempt(cxxutil::unit::TestContext *tc) override {
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
    }

    virtual void finally() override {
        delete ps;
    }
public:
    static constexpr cxxutil::unit::TestTree *ONLY = &ONLY_VAL;
};

StateCase1 StateCase1::ONLY_VAL;

static const size_t STATE_SUITE_TESTS_LEN = 1;
static cxxutil::unit::TestTree * const 
STATE_SUITE_TESTS[STATE_SUITE_TESTS_LEN] = {
    StateCase1::ONLY 
};

static cxxutil::unit::TestSuite
STATE_SUITE_VALUE("State Suite", STATE_SUITE_TESTS, STATE_SUITE_TESTS_LEN);

cxxutil::unit::TestTree * const 
tif::model::STATE_SUITE = &STATE_SUITE_VALUE;
