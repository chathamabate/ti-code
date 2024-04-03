
#pragma once

#include "./vec.h"

namespace math {
    class Line {
    private:
        Vec3D point;
        Vec3D dir;

    public:
        inline Line(const Vec3D &p, const Vec3D &d) : point(p), dir(d) {}



    };
}
