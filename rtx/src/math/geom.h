#pragma once

#include "./vec.h"
#include "./ray.h"
#include "./material.h"

namespace math {
    class Geom {
    private:
        Material mat;  
        Vec3D center;

    public:
        inline Geom(const Material &m, const Vec3D &c) 
            : mat(m), center(c) {}

        inline Material Mat() const {
            return this->mat;
        }

        inline Material *MatPtr() {
            return &(this->mat);
        }

        inline Vec3D Center() const {
            return this->center;
        }

        inline Vec3D *CenterPtr() {
            return &(this->center);
        }


        // If there is an intersection, the point of intersection
        // and the normal vector will be written to out. 
        // True will be returned.
        // 
        // otherwise, false is returned, out is unmodified.
        virtual bool intersect(Ray ray, Ray *out) const = 0;
    };
}
