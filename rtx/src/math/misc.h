
#pragma once

namespace math {

    constexpr float ERR = 0.0001f;

    inline bool isZero(float v) {
        return -ERR < v && v < ERR;
    }

    void float2str(char *buf, float val);
    void displayFloat(float val);
}
