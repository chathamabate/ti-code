
#pragma once

#include "./vec.h"

namespace math {
    class Ray {
    private:
        Vec3D point;
        Vec3D dir;

    public:
        inline Ray(const Vec3D &p, const Vec3D &d) : point(p), dir(d) {}

        inline Vec3D Point() const {
            return this->point;
        }

        inline Vec3D Dir() const {
            return this->dir;
        }
    };
}
