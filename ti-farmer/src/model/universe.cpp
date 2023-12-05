#include "./universe.h"

using namespace tif::model;


// Earth Seasons.

static const season_t EARTH_SPRING = {
    .description = "",
    .duration = 91,
};

static const season_t EARTH_SUMMER = {
    .description = "",
    .duration = 92,
};

static const season_t EARTH_FALL = {
    .description = "",
    .duration = 91,
};

static const season_t EARTH_WINTER = {
    .description = "",
    .duration = 91,
};

static const planet_t EARTH = {
    .name = "Earth",
    .description = "A small green planet with a relatively mild climate.",

    .mazeRows = 8,
    .mazeCols = 8,

    .seasons = {
        &EARTH_SPRING,
        &EARTH_SUMMER,
        &EARTH_FALL,
        &EARTH_WINTER
    }
};
