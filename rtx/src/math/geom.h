#pragma once

#include "./vec.h"
#include "./ray.h"
#include "./material.h"

namespace math {
    class Geom {
    private:
        const Material * const mat;
        Vec3D center;

    public:
        inline Geom(const Material *m, const Vec3D &c) 
            : mat(m), center(c) {}

        inline const Material *getMat() const {
            return this->mat;
        }

        inline Vec3D getCenter() const {
            return this->center;
        }

        virtual inline void setCenter(const Vec3D &c) {
            this->center = c;
        }

        // If there is an intersection, the point of intersection
        // and the normal vector will be written to outR. 
        // The parameter value will be written to outS.
        // True will be returned.
        //
        // NOTE: outR does NOT need to be normalized.
        // The given ray need not be normalized either.
        // 
        // otherwise, false is returned, outR/S are unmodified.
        virtual bool intersect(Ray ray, Ray *outR, float *outS) const = 0;
    };
}
