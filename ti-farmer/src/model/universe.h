#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace tif { namespace model {

    typedef uint32_t day_count_t;

    struct universe_t;
    struct planet_t;
    struct season_t;
    struct crop_t;

    // All structs will be referenced via their indeces in
    // their parent arrays.
    // For example the season at index 1 in its parent planet struct
    // is referenced using ID 1.

    struct universe_t {
        const char *name;

        size_t planetsLen;
        const planet_t *planets;
    };

    struct planet_t {
        // Name of this planet.
        const char *name;
        
        // Seasons are interpreted in the order they're
        // given! (Year length = the sum of season durations)
        size_t seasonsLen;
        const season_t *seasons;

        // Different crops available on said planet.
        size_t cropsLen;
        const crop_t *crops;
    };

    struct season_t {
        // Name of this season.
        const char *name;

        // How many days the season lasts.
        day_count_t duration;
    };

    struct crop_t {
        const char *name;

        // How long before the crop is ready to harvest.
        day_count_t timeToHarvest;

        // Crops can only be grown in a single season.
        // This is the id of said season.
        size_t seasonId;
    };
}}
