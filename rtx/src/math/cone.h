
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"
#include "./disk.h"

namespace math {
    class Cone : public Geom {
    private:
        const float length;
        const float length2;

        const float mag2;
        const float cos2;

        // This is the radius of the enclosing sphere!
        const float rad2;

        const Disk cap;     // Normal of the cap is the axis of 
                            // the cone.

        const Vec3D tip;    // The tip of the cone.

    public:
        inline Cone(const Vec3D &c, 
                float len, float rad,
                float theta, float phi) 
            : Cone(c, Vec3D::getNorm(theta, phi), len, rad) {};

        Cone(const Vec3D &c,
                const Vec3D &n, float l, float r);

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
