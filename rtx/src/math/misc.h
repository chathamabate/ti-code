
#pragma once

#include <stdint.h>

namespace math {

    constexpr float ERR = 0.0001f;

    inline bool isZero(float v) {
        return -ERR < v && v < ERR;
    }

    inline float intPow(float v, uint8_t p) {
        float res = 1.0f;
        for (uint8_t i = 0; i < p; i++) {
            res *= v;
        }

        return res;
    }

    void float2str(char *buf, float val);
    void displayFloat(float val);
}
