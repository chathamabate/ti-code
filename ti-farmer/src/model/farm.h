
#pragma once

#include <cxxutil/core/mem.h>

namespace tif { namespace model {

    // Farm time!
    // What is a farm??
    // How do we represent it??

    struct farm_date_t {
        cxxutil::core::U24 year;    // 0 - U24_MAX.
        uint16_t day;               // 0 - 364.
        
        inline bool operator==(farm_date_t o) const {
            return (this->year == o.year && 
                    this->day == o.year); 
        } 

        inline bool operator>(farm_date_t o) const {
            return this->year > o.year ||
                (this->year == o.year && this->day > o.day);
        }

        inline bool operator<=(farm_date_t o) const {
            return !(*this > o);
        }

        inline bool operator>=(farm_date_t o) const {
            return this->year > o.year ||
                (this->year == o.year && this->day >= o.day);
        }

        inline bool operator<(farm_date_t o) const {
            return !(*this >= o);
        }

        farm_date_t operator+(farm_date_t o) const;
        inline void operator+=(farm_date_t o) {
            farm_date_t sum = *this + o;

            this->year = sum.year;
            this->day = sum.day;
        }

        // NOTE: Be careful when using subtraction as
        // dates are unsigned.
        //
        // when doing d1 - d2, make sure d1 > d2.
        farm_date_t operator-(farm_date_t o) const;
        inline void operator-=(farm_date_t o) {
            farm_date_t diff = *this - o;

            this->year = diff.year;
            this->day = diff.day;
        }
    };
    

}}
