
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"

namespace math {
    class Disk : public Plane {
    private:
        float radius;

    public:
        inline Disk() 
            : Plane(), radius(1.0f) {}

        inline Disk(const Material *m, const Vec3D &c, 
                const Vec3D &n, float r) 
            : Plane(m, c, n), radius(r) {}

        Disk(const Material *m, const Vec3D &c,
                float r, float theta, float phi);

        inline float getRadius() const {
            return this->radius;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
