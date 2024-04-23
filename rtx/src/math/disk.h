
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"

namespace math {
    class Disk : public Plane {
    private:
        const float radius;

    public:
        inline Disk(const Vec3D &c, 
                const Vec3D &n, float r) 
            : Plane(c, n), radius(r) {}

        inline Disk(const Vec3D &c,
                float r, float theta, float phi) 
            : Plane(c, Vec3D::getNorm(theta, phi)), radius(r) {}

        inline float getRadius() const {
            return this->radius;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
