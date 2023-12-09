#include <ti-farmer/src/statics/universe.h>
#include <ti-farmer/src/statics/earth.h>

using namespace tif::statics;

static const uint8_t GOAL_TIMELINE_LEN = 5;
static const cxxutil::core::U24 
GOAL_TIMELINE_GOALS[GOAL_TIMELINE_LEN] = {
    5, 100, 500, 5000, 10000 // TBD.
};

static const goal_timeline_t GOAL_TIMELINE = {
    .timelineLen = GOAL_TIMELINE_LEN,
    .timeline = GOAL_TIMELINE_GOALS
};

static const uint8_t PLANETS_LEN = 1;
static const planet_t * const 
PLANETS[PLANETS_LEN] = {
    &EARTH
};

const universe_t tif::statics::UNIVERSE = {
    .goalTimeline = &GOAL_TIMELINE,

    .planetsLen = PLANETS_LEN,
    .planets = PLANETS
};
