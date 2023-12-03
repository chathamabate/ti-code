

#include "./farm.h"

using namespace tif::model;

farm_date_t farm_date_t::operator+(farm_date_t o) const {
    farm_date_t result = {
        .year = this->year + o.year,
        .day = (uint16_t)(this->day + o.day)
    };

    if (result.day >= 365) {
        result.day -= 365;
        result.year++;
    }

    return result;
}

farm_date_t farm_date_t::operator-(farm_date_t o) const {
    if (o.day > this->day) {
        return (farm_date_t) {
            .year = this->year - o.year - 1,
            .day = (uint16_t)(this->day - o.day + 365)
        };
    } 

    return (farm_date_t) {
        .year = this->year - o.year,
        .day = (uint16_t)(this->day - o.day)
    };
}


