
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"
#include "./disk.h"

namespace math {
    class Cylinder : public Geom {
    private: 
        const float halflen;
        const float radius;
        const Vec3D axis;

        const Disk cap1;
        const Disk cap2;

    public:
        inline Cylinder(const Material *m, const Vec3D &c, 
                float len, float r, float theta, float phi) 
            : Geom(m, c), 
            halflen(len / 2.0f),
            radius(r), 
            axis(Vec3D::getNorm(theta, phi) * this->halflen),
            cap1(m, c + axis, axis, r),
            cap2(m, c - axis, -axis, r) {}

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
