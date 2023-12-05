#include "./universe.h"

using namespace tif::model;


// Earth Seasons.

// Earth Spring.

static const crop_t CARROTS = {
    .name = "Carrots",
    .description = "A conic orange vegetable.",
    .timeToHarvest = 2,
};

static const crop_t KALE = {
    .name = "Kale",
    .description = "Healthy and unremarkable.",
    .timeToHarvest = 4,
};

static const crop_t ONIONS = {
    .name = "Onions",
    .description = "Pairs well with cow meat.",
    .timeToHarvest = 7,
};

static const crop_t POTATOES = {
    .name = "Potatoes",
    .description = "Great in a hearty stew.",
    .timeToHarvest = 12,
};

static const uint8_t EARTH_SPRING_CROPS_LEN = 4;
static const crop_t * const
EARTH_SPRING_CROPS[EARTH_SPRING_CROPS_LEN] = {
    &CARROTS,
    &KALE,
    &ONIONS,
    &POTATOES
};

static const season_t EARTH_SPRING = {
    .description = "Flowers start to bloom as snow melts away.",
    .duration = 91,

    .cropsLen = EARTH_SPRING_CROPS_LEN,
    .crops = EARTH_SPRING_CROPS
};

// Earth Summer.

static const crop_t GRAPES = {
    .name = "Grapes",
    .description = "A summer picnic necessity.",
    .timeToHarvest = 1,
};

static const crop_t BLUEBERRIES = {
    .name = "Blueberries",
    .description = "Sweet addition to a morning smoothie",
    .timeToHarvest = 2
};

static const crop_t TOMATOES = {
    .name = "Tomatoes",
    .description = "The T in BLT.",
    .timeToHarvest = 6,
};

static const crop_t MINT = {
    .name = "Mint",
    .description = "A unique and flavorful leaf.",
    .timeToHarvest = 18,
};

static const uint8_t EARTH_SUMMER_CROPS_LEN = 4;
static const crop_t * const 
EARTH_SUMMER_CROPS[EARTH_SUMMER_CROPS_LEN] = {
    &GRAPES,
    &BLUEBERRIES,
    &TOMATOES,
    &MINT
};

static const season_t EARTH_SUMMER = {
    .description = "The sun is intense during this time of year.",

    .duration = 92,

    .cropsLen = EARTH_SUMMER_CROPS_LEN,
    .crops = EARTH_SUMMER_CROPS
};

// Earth Fall.

static const crop_t SPINACH = {
    .name = "Spinach",
    .description = "Healthy and remarkable.",
    .timeToHarvest = 5
};

static const crop_t BROCCOLI = {
    .name = "Broccoli",
    .description = "Looks like a small green tree.",
    .timeToHarvest = 8
};

static const crop_t BEETS = {
    .name = "Beets",
    .description = "Impressively red root.",
    .timeToHarvest = 9,
};

static const crop_t LETTUCE = {
    .name = "Lettuce",
    .description = "The L in BLT.",
    .timeToHarvest = 10
};

static const uint8_t EARTH_FALL_CROPS_LEN = 4;
static const crop_t * const
EARTH_FALL_CROPS[EARTH_FALL_CROPS_LEN] = {
    &SPINACH,
    &BROCCOLI,
    &BEETS,
    &LETTUCE,
};

static const season_t EARTH_FALL = {
    .description = "A grey and windy period.",
    .duration = 91,

    .cropsLen = EARTH_FALL_CROPS_LEN,
    .crops = EARTH_FALL_CROPS,
};

static const season_t EARTH_WINTER = {
    .description = "A cold period of snow and ice.",
    .duration = 91,

    .cropsLen = 0,
    .crops = NULL
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
